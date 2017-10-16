/*
 * filename: node.h
 * describe: This is the definition file of the generic node file classes
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     April 28, 2009
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef NODE_H
#define NODE_H

#include <string>
#include <fstream>

#pragma warning( disable : 4290 )

typedef unsigned short int NodeNbr;
const int nodelength = 4096;
const int nodedatalength = nodelength - sizeof(NodeNbr);

// exceptions to be thrown
class BadFileOpen{};
class FileReadError{};
class FileWriteError{};

// Node File Header Record
class FileHeader  {
  NodeNbr deletednode;     // first deleted node
  NodeNbr highestnode;     // highest assigned node
  friend class NodeFile;
  FileHeader() {
    deletednode = highestnode = 0;
  }
};

// Node File Header Class
class NodeFile  {
public:
  NodeFile(const std::string& filename) throw (BadFileOpen);
  virtual ~NodeFile();

  void SetDeletedNode(NodeNbr node) {
    header.deletednode = node;
  }
  NodeNbr DeletedNode() const {
    return header.deletednode;
  }
  void SetHighestNode(NodeNbr node) {
    header.highestnode = node;
  }
  NodeNbr HighestNode() const {
    return header.highestnode;
  }
  NodeNbr NewNode();
  void ReadData(void *buf, unsigned int siz, long wh = -1) throw (FileReadError);
  void WriteData(const void *buf, unsigned int siz, long wh = -1) throw (FileWriteError);
  void Seek(std::streampos offset, std::ios::seek_dir dir = std::ios::beg) {
    nfile.seekg(offset,dir); nfile.seekp(offset,dir);
  }
  std::streampos FilePosition() {
    return nfile.tellg();
  }
  bool NewFile() const {
    return newfile;
  }
  void ResetNewFile() {
    newfile = false;
  }
private:
  FileHeader header;
  FileHeader origheader;
  std::fstream nfile;
  bool newfile;    // true if building new node file
};

// ============================
// Node Record
// ============================
class Node  {
public:
  Node(NodeFile *hd = 0, NodeNbr node = 0);
  virtual ~Node();

  Node& operator=(Node& node);
  void SetNextNode(NodeNbr node) {
    nextnode = node; MarkNodeChanged();
  }
  NodeNbr NextNode() const {
    return nextnode;
  }
  void SetNodeNbr(NodeNbr node) {
    nodenbr = node;
  }
  NodeNbr GetNodeNbr() const {
    return nodenbr;
  }
  void MarkNodeDeleted() {
    deletenode = true;
  }
  void MarkNodeChanged() {
    nodechanged = true;
  }
  bool NodeChanged() const {
    return nodechanged;
  }
  long NodeAddress();
  virtual int NodeHeaderSize() const {
    return sizeof(NodeNbr);
  }
private:
  void CloseNode();
protected:
  NodeFile *owner;
  NodeNbr nodenbr;  // current node number
  bool nodechanged; // true if the node changed
  bool deletenode;  // true if the node is being deleted
private:
  NodeNbr nextnode;
};

#endif