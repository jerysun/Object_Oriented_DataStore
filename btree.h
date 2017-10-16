/*
 * filename: btree.h
 * describe: This is the definition file of B-Tree classes, which is the core
 *           algorithm of the storage of the open source project EDS (
 *           Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     March 09, 2010
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef BTREE_H
#define BTREE_H

#include <fstream>
#include <string>
#include "linklist.h"
#include "node.h"

#pragma warning (disable: 4244)

class EdsKey;
class TRNode;
struct Class;

const int classnamesize = 32;

// IndexFile class
class IndexFile : public NodeFile {
public:
  IndexFile(const std::string &name) : NodeFile(name + ".idx") {}
};

// b-tree header record
class TreeHeader {
  TreeHeader() {
    rootnode = 0;
    keylength = 0;
  }

  friend class EdsBtree;
  friend class IndexFile;
  NodeNbr rootnode;    // node number of the root
  KeyLength keylength; // length of a key in this b-tree
};

// b-tree index
class EdsBtree {
public:
  EdsBtree(IndexFile &ndx, Class *cls, EdsKey *ky) throw(BadKeylength);
  ~EdsBtree();

  void Insert(EdsKey *keypointer);
  void Delete(EdsKey *keypointer);
  bool Find(EdsKey *keypointer);
  EdsKey *Current();
  EdsKey *First();
  EdsKey *Last();
  EdsKey *Next();
  EdsKey *Previous();
  IndexFile &GetIndexFile() const { return index; }
  EdsKey *NullKey() const { return nullkey; }
  EdsKey *MakeKeyBuffer() const;
  NodeNbr Root() const { return header.rootnode; }
  NodeNbr GetKeyLength() const { return header.keylength; }
  IndexNo Indexno() const { return indexno; }
  const Class *ClassIndexed() const { return classindexed; }
  void SetClassIndexed(Class *cid) { classindexed = cid; }
private:
  std::streampos HdrPos() {
    return classindexed->headeraddr + (std::streamoff)(indexno * sizeof(TreeHeader));
  }
  void ReadHeader() { index.ReadData(&header, sizeof(TreeHeader), HdrPos()); }
  void WriteHeader() { index.WriteData(&header, sizeof(TreeHeader), HdrPos()); }
  void SaveKeyPosition();
private:
  TreeHeader header;   // btree header
  TRNode *trnode;       // -> current node value
  EdsKey *nullkey;     // for building empty derived key
  IndexFile &index;    // index file this tree lives in
  IndexNo indexno;     // 0=primary key, > 0=secondary key
  Class *classindexed; // -> class structure of indexed class
  NodeNbr currnode;    // current node number
  NodeNbr oldcurrnode; // for repositioning
  int oldcurrkey;    //  "        "
};

// b-tree TRNode class
class TRNode : Node {
public: // due to a bug in Borland C++ 4.0
  ~TRNode();
private:
  TRNode(EdsBtree *bt, NodeNbr node);
  bool SearchNode(EdsKey *keyvalue);
  void Insert(EdsKey *keyvalue);
  int m();
  void WriteBtreeKey(EdsKey *thiskey);
  void Adopt(NodeNbr node);
  void Adoption();
  bool isLeaf() const { return header.isleaf; }
  NodeNbr Parent() const { return header.parent; }
  NodeNbr LeftSibling() const { return header.leftsibling; }
  NodeNbr RightSibling() const { return header.rightsibling; }
  int KeyCount() const { return header.keycount; }
  NodeNbr LowerNode() const { return header.lowernode; }
  bool Redistribute(NodeNbr sib);
  bool Implode(TRNode &right);
  int NodeHeaderSize() const {
    return sizeof(TRNodeHeader) + Node::NodeHeaderSize();
  }
  TRNode &operator=(TRNode &trnode);
private:
  friend class EdsBtree;
  struct TRNodeHeader {
    bool isleaf;          // true if node is a leaf
    NodeNbr parent;       // parent to this node
    NodeNbr leftsibling;  // left sibling node
    NodeNbr rightsibling; // right sibling node
    int keycount;   // number of keys in this node
    NodeNbr lowernode;    // lower node associated with
                          // keys < keys in this node
    TRNodeHeader() {
      isleaf = false;
      parent = leftsibling = rightsibling = keycount = lowernode = 0;
    }
  } header;
  EdsKey *currkey;         // current key
  EdsBtree *btree;         // btree that owns this node
  LinkedList<EdsKey> keys; // the keys in this node
};

#endif
