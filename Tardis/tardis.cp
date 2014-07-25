/*
 *  tardis.cp
 *  tardis
 *
 *  Created by DB on 7/25/14.
 *  Copyright (c) 2014 D Blalock. All rights reserved.
 *
 */

#include <iostream>
#include "tardis.h"
#include "tardisPriv.h"

void tardis::HelloWorld(const char * s)
{
	 tardisPriv *theObj = new tardisPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void tardisPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

