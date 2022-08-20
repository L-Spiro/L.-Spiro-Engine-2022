/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Sorts data by index references.  Instead of moving the actual data around, it creates an index table
 *	for that data and sorts the indices.  This is often faster than trying to sort data composed of many-byte elements.
 *	Uses a quick sort.  The data being sorted must provide < and == operators.
 */

#include "LSSTDIndexSorter.h"
