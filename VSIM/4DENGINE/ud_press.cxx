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

//--------------------------------------------------------------------------
//
//      Methods for class PRESS_UPDATE
//      Date last modified: September 27, 1996
//      Written by Frances A. Houle
//      IBM  
//
//--------------------------------------------------------------------------


#include "ud_press.hxx"
#include "simulatr.hxx"



//-----------------------------------------------------------------------------
//       Definition of UpdateState method
//       Purpose: to manage updating process for compartment pressures
//       Parameters: none
//       Returns: nothing
//-----------------------------------------------------------------------------

void press_update :: UpdateState ()

{            //begin
	selected_event_info event;

	event = Simulator->GetEventInfo();
	switch (event.Location) {

	case REACTION_COMPARTMENT:
		Simulator->UpdatePressureAfterReaction (event.ProcessAreaNo);
		break;

	default: // MASS_TRANSFER
		Simulator->UpdatePressureAfterTransfer (event.ProcessAreaNo);
		break;
	} //end switch

	return;

}           //end function

//-----------------------------------------------------------------------------


