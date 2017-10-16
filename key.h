/*
 * filename: key.h
 * describe: This is the definition file of abstract base class EdsKey, used
 *           by the datastore engine - EDatastore of my open source project:
 *           EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     December 15, 2015
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef KEY_H
#define KEY_H

#include <typeinfo>

// EdsKey abstract base class
class EdsKey {
public:
  EdsKey(NodeNbr fa = 0);
  virtual ~EdsKey() {}
  virtual int operator>(const EdsKey& key) const = 0;
  virtual int operator==(const EdsKey& key) const = 0;
  virtual EdsKey& operator=(const EdsKey& key);

  void Relate(const type_info *ti) {
    relatedclass = ti;
  }
  KeyLength GetKeyLength() const {
    return keylength;
  }
  void SetKeyLength(KeyLength kylen) {
    keylength = kylen;
  }
private:
  virtual void WriteKey(IndexFile& bfile) = 0;
  virtual void ReadKey(IndexFile& bfile) = 0;
  virtual bool isNullValue() const = 0;
  virtual void CopyKeyData(const EdsKey *key) = 0;
  virtual bool isObjectAddress() const = 0;
  virtual const ObjAddr *ObjectAddress() const = 0;
  virtual EdsKey *MakeKey() const = 0;
protected:
  const type_info *relatedclass;
  IndexNo indexno; // 0=primary key, >0 =secondary key
  KeyLength keylength;
private:
  friend class EDatastore;
  friend class EdsBtree;
  friend class TRNode;
  friend class Serialize;
  NodeNbr fileaddr;    // object address -> by this key
  NodeNbr lowernode;   // lower node of keys > this key
};

// Key class
template <class T>
class Key : public EdsKey {
public:
  Key(const T& key);
  virtual ~Key() {}

  EdsKey& operator=(const EdsKey& key);
  int operator>(const EdsKey& key) const;
  int operator==(const EdsKey& key) const;
  T& KeyValue() {
    return ky;
  }
  void SetKeyValue(const T& key) {
    ky = key;
  }
  const T& KeyValue() const {
    return ky;
  }
  virtual void WriteKey(IndexFile& ndx);
  virtual void ReadKey(IndexFile& ndx);
  bool isNullValue() const;
private:
  bool isObjectAddress() const {
    return typeid(T) == typeid(ObjAddr);
  }
  const ObjAddr *ObjectAddress() const {
    return reinterpret_cast<const ObjAddr*>(&ky);
  }
  void CopyKeyData(const EdsKey *key);
  EdsKey *MakeKey() const;
private:
  T ky;
};

template <class T>
Key<T>::Key(const T& key) : ky(key) {
  keylength = sizeof(T);
}

template <class T>
void Key<T>::CopyKeyData(const EdsKey *key) {
  const Key<T> *kp = static_cast<const Key<T>*>(key);
  ky = kp->ky;
}

template <class T>
EdsKey& Key<T>::operator=(const EdsKey& key) {
  if (this != &key) {
    EdsKey::operator=(key);
    CopyKeyData(&key);
  }
  return *this;
}

template <class T>
int Key<T>::operator>(const EdsKey& key) const {
  const Key<T> *kp = static_cast<const Key<T>*>(&key);
  return ky > kp->ky;
}

template <class T>
int Key<T>::operator==(const EdsKey& key) const {
  const Key<T> *kp = static_cast<const Key<T>*>(&key);
  return ky == kp->ky;
}

template <class T>
EdsKey *Key<T>::MakeKey() const {
  EdsKey *newkey = new Key<T>(T(0));
  newkey->SetKeyLength(keylength);
  return newkey;
}

// ReadKey must be specialized if key != simple data type
template <class T>
void Key<T>::ReadKey(IndexFile& ndx) {
  if (keylength > 0)
    ndx.ReadData(&ky, keylength);
}

// WriteKey must be specialized if key != simple data type
template <class T>
void Key<T>::WriteKey(IndexFile& ndx) {
  if (keylength > 0)
    ndx.WriteData(&ky, keylength);
}

template <class T>
bool Key<T>::isNullValue() const {
  return ky == T(0);
}

// specialized Key<string> template member functions
inline Key<std::string>::Key(const std::string& key) : ky(key) {
  keylength = (KeyLength)key.length();
}

inline void Key<std::string>::ReadKey(IndexFile& ndx) {
  char *cp = new char[keylength + 1];
  ndx.ReadData(cp, keylength);
  *(cp + keylength) = '\0';
  ky = std::string(cp);
  delete cp;
}

inline void Key<std::string>::WriteKey(IndexFile& ndx) {
  ky.resize(keylength);
  ndx.WriteData(ky.c_str(), keylength);
}

inline EdsKey *Key<std::string>::MakeKey() const {
  EdsKey *newkey = new Key<std::string>(std::string(keylength, '\0'));
  newkey->SetKeyLength(keylength);
  return newkey;
}

inline bool Key<std::string>::isNullValue() const {
  return ky.empty();
}

// Concatenated key class
template <class T1, class T2>
class CatKey : public EdsKey {
public:
  CatKey(const T1& key1, const T2& key2);
  ~CatKey() {}

  EdsKey& operator=(const EdsKey& key);
  int operator>(const EdsKey& key) const;
  int operator==(const EdsKey& key) const;

  Key<T1>& Key1() {
    return ky1;
  }
  T1& KeyValue1() {
    return ky1.KeyValue();
  }
  const T1& KeyValue1() const {
    return ky1.KeyValue();
  }
  void SetKeyValue1(const T1& key1) {
    ky1.SetKeyValue(key1);
  }
  Key<T2>& Key2() {
    return ky2;
  }
  T2& KeyValue2() {
    return ky2.KeyValue();
  }
  const T2& KeyValue2() const {
    return ky2.KeyValue();
  }
  void SetKeyValue2(const T2& key2) {
    ky2.SetKeyValue(key2);
  }
private:
  bool isObjectAddress() const {
    return false;
  }
  const ObjAddr *ObjectAddress() const {
    return 0;
  }
  void CopyKeyData(const EdsKey *key);

  // ReadKey/WriteKey must be specialized if key(s) != simple data types
  virtual void ReadKey(IndexFile& ndx) {
    ky1.ReadKey(ndx); ky2.ReadKey(ndx);
  }
  virtual void WriteKey(IndexFile& ndx) {
    ky1.WriteKey(ndx); ky2.WriteKey(ndx);
  }
  EdsKey *MakeKey() const;
  bool isNullValue() const {
    return ky1.isNullValue() && ky2.isNullValue();
  }
private:
  Key<T1> ky1;
  Key<T2> ky2;
};

template <class T1, class T2>
CatKey<T1, T2>::CatKey(const T1& key1, const T2& key2) : ky1(key1), ky2(key2) {
  keylength = ky1.GetKeyLength() + ky2.GetKeyLength();
}

template <class T1, class T2>
int CatKey<T1, T2>::operator>(const EdsKey& key) const {
  const CatKey<T1, T2> *ckp = static_cast<const CatKey<T1, T2>*>(&key);
  if (ky1 > ckp->ky1)
    return 1;
  if (ky1 == ckp->ky1 && ky2 > ckp->ky2)
    return 1;
  return 0;
}

template <class T1, class T2>
int CatKey<T1, T2>::operator==(const EdsKey& key) const {
  const CatKey<T1, T2> *ckp = static_cast<const CatKey<T1, T2>*>(&key);
  return ky1 == ckp->ky1 && ky2 == ckp->ky2;
}

template <class T1, class T2>
void CatKey<T1, T2>::CopyKeyData(const EdsKey *key) {
  const CatKey<T1, T2> *ckp = static_cast<const CatKey<T1, T2>*>(key);
  ky1 = ckp->ky1;
  ky2 = ckp->ky2;
}

template <class T1, class T2>
EdsKey& CatKey<T1, T2>::operator=(const EdsKey& key) {
  if (this != &key) {
    EdsKey::operator=(key);
    CopyKeyData(&key);
  }
  return *this;
}

template <class T1, class T2>
EdsKey *CatKey<T1, T2>::MakeKey() const {
  CatKey<T1, T2> *newkey = new CatKey<T1, T2>(T1(0), T2(0));
  newkey->ky1.SetKeyLength(ky1.GetKeyLength());
  newkey->ky2.SetKeyLength(ky2.GetKeyLength());
  newkey->SetKeyLength(keylength);
  return newkey;
}

#endif