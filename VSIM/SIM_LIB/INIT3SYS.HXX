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
//
//      Declaration of INITIALIZE_THREE_D_SYSTEM
//      This object is the class needed to initialize and run a simulation
//		using the amorphous_system simulator.
//      Date last modified: April 30, 1996
//      Written by: Frances A. Houle
//      IBM  
//
//---------------------------------------------------------------------------

#ifndef INIT3SYS_HXX
#define INIT3SYS_HXX

#include "init_sim.hxx"
#include "3d_sys.hxx"



class initialize_three_d_system : public initialize_simulator

{

protected:
	void ReadInData();

public:
	initialize_three_d_system () {}
	initialize_three_d_system ( const initialize_three_d_system& i_3_s );
	~initialize_three_d_system() {}

};


#endif