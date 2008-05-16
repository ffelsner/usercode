/**

  This macro will add histograms multiplied by their cross-section
  from a list of root files and write them
  to a target root file. The target file is newly created and must not be
  identical to one of the source files.


  Author: Sven A. Schmidt, sven.schmidt@cern.ch
  Date:   13.2.2001

  Editing Author: Michael B. Anderson, mbanderson@hep.wisc.edu
  Date:  July 12, 2007

  This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
  which had a problem with directories more than one level deep.
  (see macro hadd_old.C for this previous implementation).

  The macro from Sven has been enhanced by
     Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
   to automatically add Trees (via a chain of trees).

  To use this macro, modify the file names in function hadd.

  NB: This macro is provided as a tutorial.
      Use $ROOTSYS/bin/hadd to merge many histogram files

 */


#include <string.h>
#include <sstream>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

void findTH2( std::vector<TH2*> & outputVector,
	      TList             * sourcelist, 
	      TString             variableName, 
	      TString             sourceSuffixArray[],
	      double              crossArray[]
	      ) {
  
  TFile *first_source = (TFile*)sourcelist->First();

  TString path( (char*)strstr( first_source->GetPath(), ":" ) );
  path.Remove( 0, 2 );

  first_source->cd( path );
  TDirectory *current_sourcedir = gDirectory;
  //gain time, do not add the objects in the list in memory
  TH2::AddDirectory(kFALSE);

  // loop over all keys in this directory
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key, *oldkey=0;
  while ( (key = (TKey*)nextkey())) {

    //keep only the highest cycle number for each key
    if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

    // read object from first source file
    first_source->cd( path );
    TObject *obj = key->ReadObj();

    if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
      // descendant of TH2 -> merge it

      bool foundName = false;
      if ( variableName == obj->GetName() ) 
	foundName=true;
      
      if ( !foundName ) continue;
      cout << "finding histogram " << variableName << endl;
      TH2 *h1 = (TH2*)obj->Clone();

      std::ostringstream sourceIndex;
      TString h1Name = h1->GetName();

      // Scale by the cross-section factor
      h1->Scale(crossArray[0]);
      sourceIndex << 0;
      //      h1->SetName(h1Name+"_"+sourceIndex.str());
      h1->SetName(h1Name+"_"+sourceSuffixArray[0]);
      outputVector.push_back(h1);
      sourceIndex.str("");

      // loop over all source files and add the content of the
      // correspondant histogram to the one pointed to by "h1"
      TFile *nextsource = (TFile*)sourcelist->After( first_source );

      int q = 1; // This keeps track of which
                 // cross section factor to use

      while ( nextsource ) {
        // make sure we are at the correct directory level by cd'ing to path
        nextsource->cd( path );
        TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(variableName);
        if (key2) {
           TH2 *h2 = (TH2*)(key2->ReadObj())->Clone();
	   // Scale by the cross section factor
           // before adding.
           h2->Scale(crossArray[q]);
	   sourceIndex << q;
	   //	   h2->SetName(variableName+"_"+sourceIndex.str());
	   h2->SetName(variableName+"_"+sourceSuffixArray[q]);
	   outputVector.push_back(h2);
	   sourceIndex.str("");
           q++;
        }

        nextsource = (TFile*)sourcelist->After( nextsource );
      }
    } else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
      // it's a subdirectory
      cout << "Found subdirectory " << obj->GetName() << endl;

      findTH2( outputVector, 
	       sourcelist, 
	       variableName, 
	       sourceSuffixArray,
	       crossArray
	       );

    } else {

      // object is of no type that we know or can handle
      cout << "Unknown object type, name: "
           << obj->GetName() << " title: " << obj->GetTitle() << endl;
    }

  }

}
