/*
 * filename: btree.cpp
 * describe: This is the implementation file of B-Tree classes, which is
 *           the core algorithm of the storage of the open source project
 *           EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     March 11, 2010
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include <string>
#include "edatastore.h"

// constructor to open a btree
EdsBtree::EdsBtree(IndexFile &ndx, Class *cls, EdsKey *ky) throw(BadKeylength)
    : index(ndx) {
  nullkey = ky->MakeKey();
  nullkey->EdsKey::operator=(*ky);
  trnode = 0;
  classindexed = cls;
  currnode = 0;

  indexno = ky->indexno;

  // read the btree header
  ReadHeader();

  if (header.keylength == 0) {
    header.keylength = ky->keylength;
  } else if (ky->keylength != 0 && header.keylength != ky->keylength) {
    throw BadKeylength();
  }
}

// destructor for a btree
EdsBtree::~EdsBtree() {
  // write the btree header
  WriteHeader();
  delete trnode;
  delete nullkey;
}

// make a key buffer
EdsKey *EdsBtree::MakeKeyBuffer() const {
  EdsKey *thiskey = nullkey->MakeKey();
  thiskey->indexno = indexno;
  return thiskey;
}

// insert a key into a btree
void EdsBtree::Insert(EdsKey *keypointer) {
  // don't insert duplicate keys
  if (!Find(keypointer)) {
    EdsKey *newkey = keypointer->MakeKey();
    *newkey = *keypointer;

    NodeNbr rootnode = 0, leftnode = 0, rightnode = 0;
    bool RootisLeaf = true;

    bool done = false;
    // insert key into btree
    while (currnode) {
      int em = trnode->m();
      trnode->Insert(newkey);
      // first insertion is into leaf
      // if split, later insertions
      // are into parents (non-leaves)
      if (!trnode->header.isleaf) {
        trnode->currkey->lowernode = rightnode;
      }

      done = trnode->header.keycount <= em;
      if (!done) {
        // node is full, try to redistribute keys among siblings
        done = trnode->Redistribute(trnode->header.leftsibling);
      }

      if (!done) {
        done = trnode->Redistribute(trnode->header.rightsibling);
      }

      if (done) break;

      // cannot redistribute filled node, split it
      RootisLeaf = false;
      rightnode = index.NewNode();
      leftnode = currnode;

      TRNode right(this, rightnode);
      right.SetNodeNbr(rightnode);
      right.MarkNodeChanged();

      // establish sibling and parent relationships
      // between current node and new right sibling
      right.header.rightsibling = trnode->header.rightsibling;
      trnode->header.rightsibling = rightnode;
      right.header.leftsibling = currnode;
      right.header.parent = trnode->header.parent;

      // if the current node is a leaf, so is the new sibling
      right.header.isleaf = trnode->header.isleaf;

      // compute new key counts for the two nodes
      trnode->header.keycount = (em + 1) / 2;
      right.header.keycount = em - trnode->header.keycount;

      // locate the middle key in the current node
      EdsKey *middlekey = trnode->keys.FindEntry(trnode->header.keycount);

      // set the pointer to keys less than those in new node
      if (!right.header.isleaf) {
        right.header.lowernode = middlekey->lowernode;
      }

      // point to the keys to move (1 past middle)
      EdsKey *movekey = trnode->keys.NextEntry(middlekey);

      // middle key inserts into parent
      trnode->keys.RemoveEntry(middlekey);
      *newkey = *middlekey;
      delete middlekey;

      // move keys from current to new right node
      for (int i = 0; i < right.header.keycount; i++) {
        EdsKey *nkey = trnode->keys.NextEntry(movekey);
        trnode->keys.RemoveEntry(movekey);
        right.keys.AppendEntry(movekey);
        movekey = nkey;
      }

      // prepare to insert key into parent of split nodes
      currnode = trnode->header.parent;
      if (!currnode) {
        // no parent node, splitting the root node
        rootnode = index.NewNode();
        right.header.parent = rootnode;
        trnode->header.parent = rootnode;
      }

      // the former right sibling of the current node is now the right sibling
      // of the split node and must record the new node as left sibling
      if (right.header.rightsibling) {
        TRNode farright(this, right.header.rightsibling);
        farright.header.leftsibling = rightnode;
        farright.MarkNodeChanged();
      }

      // children of the new split node point to the current split node as
      // parent. They must be adopted by the new split node
      if (!right.header.isleaf) {
        right.Adoption();
      }

      // if splitting other than root, read parent position currkey to key
      // where split node key will be inserted
      if (currnode) {
        delete trnode; // writes the split node to disk
        // get the parent of the split nodes
        trnode = new TRNode(this, currnode);
        // position currkey where new key will insert
        trnode->SearchNode(newkey);
      }
    }

    if (!done) {
      /* new root node */
      delete trnode;
      if (rootnode == 0) {
        rootnode = index.NewNode();
      }

      trnode = new TRNode(this, rootnode);
      trnode->header.isleaf = RootisLeaf;
      currnode = header.rootnode = rootnode;
      trnode->SetNodeNbr(rootnode);
      trnode->Insert(newkey);
      trnode->header.parent = 0;
      trnode->header.keycount = 1;

      if (!RootisLeaf) {
        trnode->header.lowernode = leftnode;
        trnode->currkey->lowernode = rightnode;
      }
      trnode->MarkNodeChanged();
    }
    delete newkey;
  }
  delete trnode;
  trnode = 0;
}

void EdsBtree::SaveKeyPosition() {
  if (trnode->header.isleaf) {
    oldcurrnode = 0;
    oldcurrkey = 0;
  } else {
    oldcurrnode = currnode;
    oldcurrkey = trnode->keys.FindEntry(trnode->currkey);
  }
}

// find a key in a btree
bool EdsBtree::Find(EdsKey *keypointer) {
  oldcurrnode = 0;
  oldcurrkey = 0;

  currnode = header.rootnode;
  while (currnode) {
    delete trnode;
    trnode = new TRNode(this, currnode);

    if (trnode->SearchNode(keypointer)) {
      // search key is equal to a key in the node
      keypointer->fileaddr = trnode->currkey->fileaddr;
      oldcurrnode = 0;
      oldcurrkey = 0;
      return true;
    }

    if (trnode->currkey == trnode->keys.FirstEntry()) {
      // search key is < lowest key in node
      SaveKeyPosition();
      if (trnode->header.isleaf) break;
      currnode = trnode->header.lowernode;
    } else if (trnode->currkey) {
      // search key is < current key in node
      SaveKeyPosition();
      if (trnode->header.isleaf) break;
      currnode = trnode->keys.PrevEntry(trnode->currkey)->lowernode;
    } else {
      // search key > highest key in node
      if (trnode->header.isleaf) break;
      currnode = trnode->keys.LastEntry()->lowernode;
    }
  }
  return false;
}

// delete a key from a btree
void EdsBtree::Delete(EdsKey *keypointer) {
  if (Find(keypointer)) {
    if (!trnode->header.isleaf) {

      // if not found in leaf node, go down to leaf
      TRNode *leaf = new TRNode(this, trnode->currkey->lowernode);
      while (!leaf->header.isleaf) {
        NodeNbr lf = leaf->header.lowernode;
        delete leaf;
        leaf = new TRNode(this, lf);
      }

      // Move the left-most key from the leaf to
      // where deleted key was in higher node
      EdsKey *movekey = leaf->keys.FirstEntry();
      leaf->keys.RemoveEntry(movekey);
      leaf->header.keycount--;
      leaf->MarkNodeChanged();

      trnode->keys.InsertEntry(movekey, trnode->currkey);
      movekey->lowernode = trnode->currkey->lowernode;
      trnode->keys.RemoveEntry(trnode->currkey);
      delete trnode->currkey;
      trnode->MarkNodeChanged();
      delete trnode;

      trnode = leaf;
      trnode->currkey = trnode->keys.FirstEntry();
      currnode = trnode->GetNodeNbr();
    } else {
      // delete the key from the node
      trnode->keys.RemoveEntry(trnode->currkey);
      delete trnode->currkey;
      trnode->header.keycount--;
      trnode->MarkNodeChanged();
      if (trnode->header.keycount == 0) {
        header.rootnode = 0;
      }
    }
    // if the node shrinks to half capacity,
    //      try to combine it with a sibling node
    while (trnode->header.keycount > 0 &&
           trnode->header.keycount <= trnode->m() / 2) {
      if (trnode->header.rightsibling) {
        TRNode *right = new TRNode(this, trnode->header.rightsibling);
        if (trnode->Implode(*right)) {
          delete right;
          NodeNbr parent = trnode->header.parent;
          if (parent == 0) {
            header.rootnode = trnode->GetNodeNbr();
            break;
          }
          delete trnode;
          trnode = new TRNode(this, parent);
          continue;
        }
        delete right;
      }
      if (trnode->header.leftsibling) {
        TRNode *left = new TRNode(this, trnode->header.leftsibling);
        if (left->Implode(*trnode)) {
          delete trnode;
          NodeNbr parent = left->header.parent;
          if (parent == 0) {
            header.rootnode = left->GetNodeNbr();
            trnode = left;
            break;
          }
          delete left;
          trnode = new TRNode(this, parent);
          continue;
        }
        delete left;
      }

      // could not combine with either sibling,
      //     try to redistribute
      if (!trnode->Redistribute(trnode->header.leftsibling)) {
        trnode->Redistribute(trnode->header.rightsibling);
      }
      break;
    }
  }
  delete trnode;
  trnode = 0;
}

// return the address of the current key
EdsKey *EdsBtree::Current() {
  if (trnode == 0) {
    return 0;
  }

  if (oldcurrnode != 0) {
    currnode = oldcurrnode;
    delete trnode;
    trnode = new TRNode(this, currnode);
    trnode->currkey = trnode->keys.FindEntry(oldcurrkey);
    oldcurrnode = 0;
    oldcurrkey = 0;
  }
  return trnode->currkey;
}

// return the address of the first key
EdsKey *EdsBtree::First() {
  currnode = header.rootnode;
  if (currnode) {
    delete trnode;
    trnode = new TRNode(this, currnode);
    while (!trnode->header.isleaf) {
      currnode = trnode->header.lowernode;
      delete trnode;
      trnode = new TRNode(this, currnode);
    }
    trnode->currkey = trnode->keys.FirstEntry();
  }
  return Current();
}

// return the address of the last key
EdsKey *EdsBtree::Last() {
  currnode = header.rootnode;
  if (currnode) {
    delete trnode;
    trnode = new TRNode(this, currnode);
    while (!trnode->header.isleaf) {
      currnode = trnode->keys.LastEntry()->lowernode;
      delete trnode;
      trnode = new TRNode(this, currnode);
    }
    trnode->currkey = trnode->keys.LastEntry();
  }
  return Current();
}

// return the address of the next key
EdsKey *EdsBtree::Next() {
  if (trnode == 0 || trnode->currkey == 0) {
    return First();
  }

  if (!trnode->header.isleaf) {
    // current key is not in a leaf
    currnode = trnode->currkey->lowernode;
    delete trnode;
    trnode = new TRNode(this, currnode);
    // go down to the leaf
    while (!trnode->header.isleaf) {
      currnode = trnode->header.lowernode;
      delete trnode;
      trnode = new TRNode(this, currnode);
    }
    // use the first key in the leaf as the next one
    trnode->currkey = trnode->keys.FirstEntry();
  } else {
    // current key is in a leaf
    EdsKey *thiskey = nullkey->MakeKey();
    *thiskey = *(trnode->currkey);

    // point to the next key in the leaf
    trnode->currkey = trnode->keys.NextEntry(trnode->currkey);
    while (trnode->currkey == 0 && currnode != header.rootnode) {
      // current key was the last one in the leaf
      TRNode pnode(this, trnode->Parent());
      pnode.SearchNode(thiskey);
      currnode = pnode.GetNodeNbr();
      *trnode = pnode;
    }
    delete thiskey;
  }
  return Current();
}

// return the address of the previous key
EdsKey *EdsBtree::Previous() {
  if (trnode == 0 || trnode->currkey == 0) {
    return Last();
  }

  if (!trnode->header.isleaf) {
    // current key is not in a leaf
    EdsKey *ky = trnode->keys.PrevEntry(trnode->currkey);
    if (ky != 0) {
      currnode = ky->lowernode;
    } else {
      currnode = trnode->header.lowernode;
    }

    delete trnode;
    trnode = new TRNode(this, currnode);
    // go down to the leaf
    while (!trnode->header.isleaf) {
      currnode = trnode->keys.LastEntry()->lowernode;
      delete trnode;
      trnode = new TRNode(this, currnode);
    }
    // use the last key in the leaf as the next one
    trnode->currkey = trnode->keys.LastEntry();
  } else {
    // current key is in a leaf
    EdsKey *thiskey = nullkey->MakeKey();
    *thiskey = *(trnode->currkey);

    // point to the previous key in the leaf
    trnode->currkey = trnode->keys.PrevEntry(trnode->currkey);
    while (trnode->currkey == 0 && currnode != header.rootnode) {
      // current key was the first one in the leaf
      TRNode pnode(this, trnode->Parent());
      pnode.SearchNode(thiskey);

      if (pnode.currkey == 0) {
        pnode.currkey = pnode.keys.LastEntry();
      } else {
        pnode.currkey = pnode.keys.PrevEntry(pnode.currkey);
      }

      currnode = pnode.GetNodeNbr();
      *trnode = pnode;
    }
    delete thiskey;
  }
  return Current();
}
