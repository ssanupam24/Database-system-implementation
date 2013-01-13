#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include "Errors.h"

DBFile::DBFile(): curPageIdx(0) {}

int DBFile::Create (char *fpath, fType ftype, void *startup) {
  theFile.Open(0, fpath);
  return 1;  // TODO: check: always succeed?
}

void DBFile::Load (Schema &myschema, char *loadpath) {
  FILE* ifp = fopen(loadpath, "r");
  FATALIF(ifp==NULL, loadpath);

  Record next;
  while (next.SuckNextRecord(&myschema, ifp)) {
    Add(next);
  }
}

int DBFile::Open (char *fpath) {
  theFile.Open(1, fpath);
  return 1;  // TODO: check: always succeed?
}

void DBFile::MoveFirst () {
  curPageIdx = 0;
  theFile.GetPage(&curPage, curPageIdx);
}

int DBFile::Close () {
  return theFile.Close();
}

void DBFile::Add (Record &rec) {
  if(theFile.GetLength()==0) return theFile.addRecordToNewPage(&rec);
  else {
    Page lastPage;
    theFile.GetPage(&lastPage, theFile.GetLength()-2); // first page contains no data
    if(!lastPage.Append(&rec)) // need to create a new page
      theFile.addRecordToNewPage(&rec);
  }
}

int DBFile::GetNext (Record &fetchme) {
  while (curPage.GetFirst(&fetchme)==0) {
    if(++curPageIdx >= theFile.GetLength()-1) return 0;  // no more records
    theFile.GetPage(&curPage, curPageIdx);
  }
  return 1;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
  ComparisonEngine comp;
  while(GetNext(fetchme)) {
    if(comp.Compare(&fetchme, &literal, &cnf)) return 1;
  }
  return 0;
}
