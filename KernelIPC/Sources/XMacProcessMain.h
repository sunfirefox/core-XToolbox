/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
#ifndef __XMacProcessMain__
#define __XMacProcessMain__

#if !VERSION_MACHO
// See _dyld_register_func_for_link_module for comments
extern "C" {
	int	initialize ();
	int	_start ();
}

int	initialize ()
{
	return _start();
}

#endif	// VERSION_MACHO


#if USE_TOOLBOX_MAIN

// Process main entry point
extern "C" {
	int	main (); 
}


int	main ()
{
	// Add any kernel init code here
	
	// Call user entry point
	ProcessMain();
}

#endif	// USE_TOOLBOX_MAIN

#endif
