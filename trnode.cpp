/*
 * filename: trnode.cpp
 * describe: This is the implementation of the B-Tree Tree node class,
 *           used by the datastore engine - EDatastore for the open
 *           source project EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     June 17, 2016
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include "edatastore.h"

TRNode::TRNode(EdsBtree *bt, NodeNbr nd) : Node(&(bt->GetIndexFile()), nd) {
  btree = bt;
  currkey = 0;
  IndexFile &nx = btree->GetIndexFile();
  long nad = NodeAddress() + Node::NodeHeaderSize();

  // read the header
  try {
    nx.ReadData(&header, sizeof(TRNodeHeader), nad);
  } catch (FileReadError) {
    // appending a new node
    nx.WriteData(&header, sizeof(TRNodeHeader), nad);
    return;
  }

  // reading an existing node, read the keys
  for (int i = 0; i < header.keycount; i++) {
    // get memory for and read a key
    EdsKey *thiskey = btree->MakeKeyBuffer();
    thiskey->ReadKey(nx);

    // read the key's file address
    NodeNbr fa;
    nx.ReadData(&fa, sizeof(NodeNbr));
    thiskey->fileaddr = fa;

    if (!header.isleaf) {
      NodeNbr lnode;
      nx.ReadData(&lnode, sizeof(NodeNbr));
      thiskey->lowernode = lnode;
    }
    keys.AppendEntry(thiskey);
  }
}

// write a key to the node's disk record
void TRNode::WriteBtreeKey(EdsKey *thiskey) {
  IndexFile &nx = btree->GetIndexFile();
  // write the key value
  thiskey->WriteKey(nx);
  // write the key's file address
  NodeNbr fa = thiskey->fileaddr;
  nx.WriteData(&fa, sizeof(NodeNbr));
  if (!header.isleaf) {
    // write the lower node pointer for non-leaf keys
    NodeNbr lnode = thiskey->lowernode;
    nx.WriteData(&lnode, sizeof(NodeNbr));
  }
}

TRNode::~TRNode() {
  if (header.keycount == 0)
    // this node is to be deleted
    deletenode = true;
  else {
    IndexFile &nx = btree->GetIndexFile();
    if (nodechanged) {
      long nad = NodeAddress() + Node::NodeHeaderSize();
      // write the node header
      nx.WriteData(&header, sizeof(TRNodeHeader), nad);
    }

    // write the keys
    EdsKey *thiskey = keys.FirstEntry();
    while (thiskey != 0) {
      if (nodechanged) {
        WriteBtreeKey(thiskey);
      }
      delete thiskey;
      thiskey = keys.NextEntry();
    }

    if (nodechanged) {
      // pad the node
      int keyspace = header.keycount * (btree->GetKeyLength() + sizeof(NodeNbr) +
                           (header.isleaf ? 0 : sizeof(NodeNbr)));
      int residual = nodedatalength - keyspace - sizeof(TRNodeHeader);
      char *fill = new char[residual];
      memset(fill, 0, residual);
      nx.WriteData(fill, residual);
      delete fill;
    }
  }
}

// assignment operator
TRNode &TRNode::operator=(TRNode &trnode) {
  EdsKey *thiskey = keys.FirstEntry();
  // if receiver has any keys, delete them
  while (header.keycount > 0) {
    delete thiskey;
    --header.keycount;
    thiskey = keys.NextEntry();
  }
  keys.ClearList();
  Node::operator=(trnode);
  header = trnode.header;
  currkey = 0;
  // copy the keys
  thiskey = trnode.keys.FirstEntry();
  while (thiskey != 0) {
    EdsKey *newkey = btree->MakeKeyBuffer();
    *newkey = *thiskey;
    keys.AppendEntry(newkey);
    if (thiskey == trnode.currkey) {
      currkey = newkey;
    }
    thiskey = trnode.keys.NextEntry();
  }
  return *this;
}

// compute m value of node
int TRNode::m() {
  int keyspace = nodelength - NodeHeaderSize();
  int keylen = btree->GetKeyLength();
  if (!header.isleaf) {
    keylen += sizeof(NodeNbr);
  }
  return keyspace / keylen;
}

// search a node for a match on a key
bool TRNode::SearchNode(EdsKey *keyvalue) {
  currkey = keys.FirstEntry();
  while (currkey != 0) {
    if (*currkey > *keyvalue) break;
    if (*currkey == *keyvalue) {
      if (keyvalue->indexno == 0)
        return true;
      if (currkey->fileaddr == keyvalue->fileaddr)
        return true;
      if (keyvalue->fileaddr == 0)
        return true;
      if (currkey->fileaddr > keyvalue->fileaddr)
        break;
    }
    currkey = keys.NextEntry();
  }
  return false;
}

void TRNode::Insert(EdsKey *keyvalue) {
  // insert the new key
  EdsKey *ky = keyvalue->MakeKey();
  *ky = *keyvalue;

  if (currkey == 0) {
    keys.AppendEntry(ky);
  } else {
    keys.InsertEntry(ky, currkey);
  }

  header.keycount++;
  nodechanged = true;
  currkey = ky;
}

// a node "adopts" all its children by telling
//      them to point to it as their parent
void TRNode::Adoption() {
  Adopt(header.lowernode);
  EdsKey *thiskey = keys.FirstEntry();
  for (int i = 0; i < header.keycount; i++) {
    Adopt(thiskey->lowernode);
    thiskey = keys.NextEntry();
  }
}

// adopt a child node
void TRNode::Adopt(NodeNbr node) {
  if (node) {
    TRNode nd(btree, node);
    nd.header.parent = nodenbr;
    nd.nodechanged = true;
  }
}

// redistribute keys among two sibling nodes
bool TRNode::Redistribute(NodeNbr sib) {
  if (sib == 0) return false;

  TRNode sibling(btree, sib);
  if (sibling.header.parent != header.parent) return false;

  int totkeys = header.keycount + sibling.header.keycount;
  if (totkeys >= m() * 2) return false;

  // assign left and right associations
  TRNode *left, *right;
  if (sib == header.leftsibling) {
    right = this;
    left = &sibling;
  } else {
    right = &sibling;
    left = this;
  }

  // compute number of keys to be in left node
  int leftct = (left->header.keycount + right->header.keycount) / 2;
  // if no redistribution would occur
  if (leftct == left->header.keycount) return false;

  // compute number of keys to be in right node
  int rightct = (left->header.keycount + right->header.keycount) - leftct;
  // get the parent
  TRNode parent(btree, left->header.parent);
  // position parent's currkey to one that points to siblings
  parent.SearchNode(left->keys.FirstEntry());
  // will move keys from left to right or right to left depending on which
  // node has the greater number of keys to start with.
  if (left->header.keycount < right->header.keycount) {
    // moving keys from right to left
    int mvkeys = right->header.keycount - rightct - 1;
    // move key from parent to end of left node
    left->currkey = parent.currkey;
    parent.currkey = parent.keys.NextEntry(parent.currkey);
    // remove parent key from its list
    parent.keys.RemoveEntry(left->currkey);
    // put it in left node's list
    left->keys.AppendEntry(left->currkey);

    if (!left->header.isleaf) {
      left->currkey->lowernode = right->header.lowernode;
    }

    // point to the keys to move (at front of right node)
    EdsKey *movekey = right->keys.FirstEntry();
    // move keys from right to left node
    for (int i = 0; i < mvkeys; i++) {
      EdsKey *nkey = right->keys.NextEntry(movekey);
      right->keys.RemoveEntry(movekey);
      left->keys.AppendEntry(movekey);
      movekey = nkey;
    }

    // move separating key from right node to parent
    right->keys.RemoveEntry(movekey);
    parent.keys.InsertEntry(movekey, parent.currkey);
    if (!right->header.isleaf)
      right->header.lowernode = movekey->lowernode;

    movekey->lowernode = right->nodenbr;
    right->header.keycount = rightct;
    left->header.keycount = leftct;
    if (!left->header.isleaf) {
      left->Adoption();
    }
  } else {
    // moving from left to right
    int mvkeys = left->header.keycount - leftct - 1;
    // move key from parent to right node
    right->currkey = parent.currkey;
    parent.currkey = parent.keys.NextEntry(parent.currkey);
    // remove parent key from its list
    parent.keys.RemoveEntry(right->currkey);
    // put it in right node's list
    right->keys.InsertEntry(right->currkey, right->keys.FirstEntry());
    if (!right->header.isleaf) {
      right->currkey->lowernode = right->header.lowernode;
    }

    // locate the first key to move in the left node
    EdsKey *movekey = left->keys.FindEntry(leftct);
    // remember the key after the one being moved up
    EdsKey *nkey = left->keys.NextEntry(movekey);
    // move key from left node up to parent
    left->keys.RemoveEntry(movekey);
    parent.keys.InsertEntry(movekey, parent.currkey);

    right->header.lowernode = movekey->lowernode;
    movekey->lowernode = right->nodenbr;
    movekey = nkey;
    // move keys from the left node to the right node
    EdsKey *inskey = right->keys.FirstEntry();
    for (int i = 0; i < mvkeys; i++) {
      EdsKey *nkey = left->keys.NextEntry(movekey);
      left->keys.RemoveEntry(movekey);
      right->keys.InsertEntry(movekey, inskey);
      movekey = nkey;
    }

    right->header.keycount = rightct;
    left->header.keycount = leftct;
    if (!right->header.isleaf) {
      right->Adoption();
    }
  }
  nodechanged = sibling.nodechanged = parent.nodechanged = true;
  return true;
}

// implode the keys of two sibling nodes
bool TRNode::Implode(TRNode &right) {
  int totkeys = right.header.keycount + header.keycount;
  if (totkeys >= m() || right.header.parent != header.parent) {
    return false;
  }

  nodechanged = right.nodechanged = true;
  header.rightsibling = right.header.rightsibling;
  header.keycount += right.header.keycount + 1;
  right.header.keycount = 0;
  // get the parent of the imploding nodes
  TRNode parent(btree, header.parent);
  parent.nodechanged = true;
  // position parent's currkey to key that points to siblings
  parent.SearchNode(keys.FirstEntry());
  // move the parent's key to the left sibling
  parent.keys.RemoveEntry(currkey);
  keys.AppendEntry(parent.currkey);
  parent.currkey->lowernode = right.header.lowernode;
  parent.header.keycount--;
  if (parent.header.keycount == 0) {
    // combined the last two leaf nodes into a new root
    header.parent = 0;
  }

  // move the keys from the right sibling into the left
  EdsKey *movekey = right.keys.FirstEntry();
  while (movekey != 0) {
    EdsKey *nkey = right.keys.NextEntry(movekey);
    right.keys.RemoveEntry(movekey);
    keys.AppendEntry(movekey);
    movekey = nkey;
  }

  if (header.rightsibling) {
    // - point right sibling of old right to imploded node
    TRNode farright(btree, header.rightsibling);
    farright.header.leftsibling = GetNodeNbr();
    farright.nodechanged = true;
  }
  Adoption();
  return true;
}
