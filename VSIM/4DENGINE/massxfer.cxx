// ----------------------------------------------------------------------------
//
// Copyright (C) 1996, 1998, 2012 International Business Machines Corporation
//   
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Methods for derived class MASS_TRANSFER
//      Date last modified January 18, 1997
//      Written by Frances A. Houle
//      IBM  
//---------------------------------------------------------------------------


#include "massxfer.hxx"
#include "simulatr.hxx"


//---------------------------------------------------------------------------
//	Constructor
//---------------------------------------------------------------------------

mass_transfer :: mass_transfer ()
: process ()

{

}


//---------------------------------------------------------------------------
//    Definition of function ProbCalc for reversible mass transfer
//     Purpose: this is a general function for calculating a rate from
//     rate law parameters. New values for the probabilities are set
//    Returns: nothing
//---------------------------------------------------------------------------

void mass_transfer :: ProbCalc()

{  				  // begin function

     FLOAT64 		q, np, T, Order;
     UINT16 		ReactantCounter;
     UINT16 		SpeciesNumber;
     BOOL8 		EnoughReactants;
     ParticleType 	nparticles;


     // first see if there are going to be enough reactants in source
     // compartment

     ReactantCounter = 0;
     EnoughReactants = TRUE;

     while ( ReactantCounter < ProcessData.numreactants )
     {
	  SpeciesNumber = ProcessData.ReactantsInStep[ReactantCounter];
	  nparticles = pSource->GetSpeciesCount( SpeciesNumber );
	  if ( nparticles < ( ParticleType )( ProcessData.ReactantStoich[ReactantCounter] ) )
	  {
	       EnoughReactants = FALSE;
	       break;
	  }                            // end if
	  ReactantCounter++;
     }                                 // endwhile ReactantCounter


     if ( EnoughReactants )

     {
	     T = pProcessArea->GetTemperature();
	     q = GetRateConstant( T );

	     //convention for mass transfer is that the transferred species is
	     //first in the list of kinetic dependencies

	     ReactantCounter = 0;
	     Order = ProcessData.ReactantOrder[ReactantCounter];

	     // if Order == 0 then use the rate coefficient only, otherwise calculate
	     // concentration dependence for transferred species. Check values to
	     // avoid unnecessary floating point calcs

	     if (Order == 1.0)
	     {
		  SpeciesNumber = ProcessData.SpeciesInRateLaw[ReactantCounter];
		  np = ( FLOAT64 ) (pSource->GetSpeciesCount(SpeciesNumber));
		  q *= np;
	     }

	     if ((Order != 0.0) && (Order != 1.0))
	     {
		  SpeciesNumber = ProcessData.SpeciesInRateLaw[ReactantCounter];
		  np = ( FLOAT64 ) (pSource->GetSpeciesCount(SpeciesNumber));
		  q *= ( pow( np, ProcessData.ReactantOrder[ReactantCounter] ) );
	     }

	     // proceed to add other concentration dependences if specified

	     ReactantCounter = 1;

	     while ( ReactantCounter < ProcessData.numSpeciesInRateLaw )
	     {
		  SpeciesNumber = ProcessData.SpeciesInRateLaw[ReactantCounter];
		  np = ( FLOAT64 ) (pSource->GetSpeciesCount(SpeciesNumber));
		  Order = ProcessData.ReactantOrder[ReactantCounter];

		  // check values of order to avoid unnecessary floating point calcs
		  if (Order == 1.0)
		  {
			  q *= np;
		  }

		  if ((Order != 0.0) && (Order != 1.0))
		  {
			  q *= ( pow( np, ProcessData.ReactantOrder[ReactantCounter] ) );
		  }

		  ReactantCounter++;
	     }                                 // endwhile ReactantCounter

	     // set new value for probability
	     Probability = q;

	} else {   // not enough reactants

		ZeroProbability();

	}	// end if EnoughReactants

	return;

}                                      // end function


//-----------------------------------------------------------------------------
//	Definition of input operator >>
//	Purpose: to read in and set up reaction step data
//	Parameters: file name and object pointer
//	Returns: file pointer
//-----------------------------------------------------------------------------

TextInputStream& operator >> ( TextInputStream& rTIS, mass_transfer& rObject )

{
	UINT16 j;
	FLOAT64 Dummy, Order;

	// for now the data will only read in one species name, it is assumed there
	// is no explicit kinetic dependence of the rate of mass transfer on other species
	// it is also assumed that only one particle of the species is transferred at a time
	// but that the kinetic order can be different from 1.
	// j is the array index for the appropriate data

	j=0;

	while ( DataCode != IDType (END_TRANSFER_STEP_INIT) )

	{

		rTIS >> DataCode;

		switch (DataCode) {

		case XFER_STEP_DATA:
		       // one species is transferred per step
			rObject.ProcessData.numreactants = 1;
			rObject.ProcessData.numproducts = 1;
			rObject.ProcessData.ReactantStoich[j] = 1;
			rObject.ProcessData.ProductStoich[j] = 1;

		       // read in species ID and copy into product data
		       rTIS >> rObject.ProcessData.ReactantsInStep[0];
		       rObject.ProcessData.ProductsInStep[0] = rObject.ProcessData.ReactantsInStep[0];

			// read enum into temp variable and type cast
			UINT16 temp;
			rTIS >> temp;
			rObject.RateConstant.Format = ( RATE_CONST_FORMAT )temp;

			rTIS >> rObject.ProcessData.Reversible;
			break;

		case XFER_RATE_CONSTANT_INIT:
			// allocate memory for rate constant
			rObject.RateConstant = new k_info;
			if ( rObject.RateConstant == 0 )
			{
				Simulator->SetStatusCode( SIM_TERMINATE_MEM_ALLOC_ERROR );
				return rTIS;
			}

			// read in next line - tells type - and store in k_info
			rTIS >> DataCode;
			ASSERT ( DataCode == IDType( XFER_RATE_CONSTANT_TYPE ) );

			rTIS >> temp;
			kFormat = ( enum RATE_CONSTANT_FORMAT )temp;
			rObject.RateConstant->SetFormat( kFormat );

			// store pointer to this step
			rObject.RateConstant->SetStepPointer( &rObject );
			switch ( kFormat ): {

				case TEMP_INDEPENDENT:
				case TEMP_DEPENDENT:
					// read in data for generic reaction/mass transfer rate constant
					rTIS >> *( (reaction_transfer_k*) )rObject.RateConstant;
					break;

				case PROGRAMMED:
					// read in data for programmed rate constant
					rTIS >> *( (programmed_k*) )rObject.RateConstant;
					break;

				default:
					break;

			}
			break;

		case XFER_STEP_RATE_LAW:
			rTIS >> Order;

			// set up forward kinetics
			rObject.ProcessData.SpeciesInRateLaw[j] = rObject.ProcessData.ReactantsInStep[j];
			rObject.ProcessData.numSpeciesInRateLaw = 1;  //for now

			if ( Order != 0.0 )
			{
				rObject.ProcessData.ReactantOrder[j] = Order;  // set to zero in constructor
			}

			if (rObject.ProcessData.Reversible)
			{
				// allocate temporary storage for reverse step data
				rObject.InitTempStepData();

				rTIS >> Order;

				// fill in the rest of the TemporaryStep data structure
				// store stoichiometry
				rObject.TemporaryStep->ReactantsInStep[j] = rObject.ProcessData.ProductsInStep[j];
				rObject.TemporaryStep->ReactantStoich[j] = rObject.ProcessData.ProductStoich[j];
				rObject.TemporaryStep->ProductsInStep[j] = rObject.ProcessData.ReactantsInStep[j];
				rObject.TemporaryStep->ProductStoich[j] = rObject.ProcessData.ReactantStoich[j];

				// Store kinetics data for reverse step
				rObject.TemporaryStep->SpeciesInRateLaw[j] = rObject.TemporaryStep->ReactantsInStep[j];
				rObject.TemporaryStep->numSpeciesInRateLaw = 1;  // for now

				if ( Order != 0.0 )
				{
					rObject.TemporaryStep->ReactantOrder[j] = Order;   // set to zero in constructor
				}

				//finish setting up data for reverse step
				rObject.TemporaryStep->numreactants = rObject.ProcessData.numproducts;
				rObject.TemporaryStep->numproducts = rObject.ProcessData.numreactants;
				rObject.TemporaryStep->Reversible = TRUE;
			}
			else
			{	// read and discard
				rTIS >> Dummy;
			}

			break;

		default:
		if ( DataCode != IDType (END_TRANSFER_STEP_INIT) )
	       {
		Simulator->SetStatusCode( SIM_TERMINATE_INPUT_ERROR );
	       }
			break;

		}   // end switch


	} // end while

	return rTIS;

}       // end >>


//---------------------------------------------------------------------------
