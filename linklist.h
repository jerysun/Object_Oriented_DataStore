/*
 * filename: linklist.h
 * describe: This is a high performance generic template linked list class
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     September 07, 2008
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

// linklist.h
// a template for a linked list

#ifndef LINKLIST_H
#define LINKLIST_H

// the linked list entry
template <class T> class ListEntry;

template <class T>
class LinkedList {
public:
  LinkedList();
  virtual ~LinkedList() { ClearList(); }

  void AppendEntry(T *entry);
  void InsertEntry(T *entry, T *curr);
  void RemoveEntry(T *entry);
  T *FindEntry(int pos);
  T *FirstEntry();
  int FindEntry(T *entry);
  T *LastEntry();
  T *NextEntry();
  T *PrevEntry();
  T *NextEntry(T *entry);
  T *PrevEntry(T *entry);
  void ClearList();
private:
  T *CurrentEntry();
  void RemoveEntry(ListEntry<T> *entry);
  void InsertEntry(T *entry);
  void InsertEntry(T *entry, int pos);
  void RemoveEntry(int pos);
private:
  // the listhead
  ListEntry<T> *firstentry;
  ListEntry<T> *lastentry;
  ListEntry<T> *iterator;
};

template <class T>
LinkedList<T>::LinkedList() {
  iterator = 0;
  firstentry = 0;
  lastentry = 0;
}

// remove all entries from a linked list
template <class T>
void LinkedList<T>::ClearList() {
  ListEntry<T> *lentry = firstentry;
  while (lentry != 0) {
    ListEntry<T> *nxt = lentry->nextentry;
    delete lentry;
    lentry = nxt;
  }
  iterator = 0;
  firstentry = 0;
  lastentry = 0;
}

// the linked list entry
template <class T> class ListEntry {
  ListEntry(T *entry);
  friend class LinkedList<T>;
  T *thisentry;
  ListEntry<T> *nextentry;
  ListEntry<T> *preventry;
};

// construct a linked list entry
template <class T> ListEntry<T>::ListEntry(T *entry) {
  thisentry = entry;
  nextentry = 0;
  preventry = 0;
}

// append an entry to the linked list
template <class T>
void LinkedList<T>::AppendEntry(T *entry) {
  ListEntry<T> *newentry = new ListEntry<T>(entry);
  newentry->preventry = lastentry;

  if (lastentry) {
    lastentry->nextentry = newentry;
  }

  if (firstentry == 0) {
    firstentry = newentry;
  }
  lastentry = newentry;
}

// return the current linked list entry
template <class T>
T *LinkedList<T>::CurrentEntry() {
  return iterator ? iterator->thisentry : 0;
}

// return the first entry in the linked list
template <class T>
T *LinkedList<T>::FirstEntry() {
  iterator = firstentry;
  return CurrentEntry();
}

// return the last entry in the linked list
template <class T>
T *LinkedList<T>::LastEntry() {
  iterator = lastentry;
  return CurrentEntry();
}

// return the next entry following the specified one
template <class T>
T *LinkedList<T>::NextEntry(T *entry) {
  FindEntry(entry);
  return NextEntry();
}

// return the next entry in the linked list
template <class T>
T *LinkedList<T>::NextEntry() {
  if (iterator == 0) {
    iterator = firstentry;
  } else {
    iterator = iterator->nextentry;
  }
  return CurrentEntry();
}

// return the previous entry ahead of the specified one
template <class T>
T *LinkedList<T>::PrevEntry(T *entry) {
  FindEntry(entry);
  return PrevEntry();
}

// return the previous entry in the linked list
template <class T>
T *LinkedList<T>::PrevEntry() {
  if (iterator == 0) {
    iterator = lastentry;
  } else {
    iterator = iterator->preventry;
  }
  return CurrentEntry();
}

// remove an entry from the linked list by position
template <class T>
void LinkedList<T>::RemoveEntry(int pos) {
  FindEntry(pos);
  if (iterator != 0) {
    RemoveEntry(iterator);
  }
}

// remove an entry from the linked list by entry address
template <class T>
void LinkedList<T>::RemoveEntry(ListEntry<T> *lentry) {
  if (lentry == 0) {
    return;
  }

  if (lentry == iterator) {
    iterator = lentry->preventry;
  }

  // repair any break made by this removal
  if (lentry->nextentry) {
    lentry->nextentry->preventry = lentry->preventry;
  }

  if (lentry->preventry) {
    lentry->preventry->nextentry = lentry->nextentry;
  }
  // maintain listhead if this is last and/or first
  if (lentry == lastentry) {
    lastentry = lentry->preventry;
  }

  if (lentry == firstentry) {
    firstentry = lentry->nextentry;
  }
  delete lentry;
}

// remove an entry from the linked list by entry
template <class T>
void LinkedList<T>::RemoveEntry(T *entry) {
  FindEntry(entry);
  RemoveEntry(iterator);
}

// insert an entry into the linked list ahead of another
template <class T>
void LinkedList<T>::InsertEntry(T *entry, T *curr) {
  FindEntry(curr);
  InsertEntry(entry);
}

// insert an entry into the linked list by position
template <class T>
void LinkedList<T>::InsertEntry(T *entry, int pos) {
  FindEntry(pos);
  InsertEntry(entry);
}

// insert an entry into the linked list ahead of iterator
template <class T>
void LinkedList<T>::InsertEntry(T *entry) {
  if (iterator == 0) {
    AppendEntry(entry);
  } else {
    ListEntry<T> *newentry = new ListEntry<T>(entry);
    newentry->nextentry = iterator;
    if (iterator) {
      newentry->preventry = iterator->preventry;
      iterator->preventry = newentry;
    }
    if (newentry->preventry) {
      newentry->preventry->nextentry = newentry;
    }
    if (iterator == firstentry) {
      firstentry = newentry;
    }
    iterator = newentry;
  }
}

// return a specific linked list entry
template <class T>
T *LinkedList<T>::FindEntry(int pos) {
  iterator = firstentry;
  while (iterator && pos--) {
    iterator = iterator->nextentry;
  }
  return CurrentEntry();
}

// return a specific linked list entry number
template <class T>
int LinkedList<T>::FindEntry(T *entry) {
  int pos = 0;

  if (entry != 0) {
    iterator = firstentry;
    while (iterator) {
      if (entry == iterator->thisentry) break;
      iterator = iterator->nextentry;
      pos++;
    }
  }
  return pos;
}

#endif
