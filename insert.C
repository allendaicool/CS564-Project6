#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
/*  Insert a tuple with the given attribute values (in attrList) in relation. The value of the attribute is supplied in the attrValue member of the attrInfo structure. Since the order of the attributes in attrList[] may not be the same as in the relation, you might have to rearrange them before insertion. If no value is specified for an attribute, you should reject the insertion as Minirel does not implement NULLs.
 */
    
    //Check for NULLs in attrList
    for(int k=0; k < attrCnt; k ++)
    {
        if(attrList[k].attrValue == NULL)
        {
            return ATTRNOTFOUND;  //If NULL attr, reject the insertion.
        }
    }
    
    
    Status status;
    RelDesc rd;
    AttrDesc *attrs;
    int allAttrCnt;
    Record rec;
    
    // create buffers for variable conversion
    int intBuf;       
    float floatBuf;

    
    // get relation data
    if ((status = relCat->getInfo(relation, rd)) != OK) return status;
    
    // get attribute data
    if ((status = attrCat->getRelInfo(rd.relName, allAttrCnt, attrs)) != OK)
        return status;
    
    //calculate the total record length
    rec.length = 0;
    for(int i = 0; i < attrCnt; i ++)
    {
        rec.length += attrs[i].attrLen;
    }

    // allocate space for the record
    char outData[rec.length];
    rec.data = &outData;

    // create pointer to be used for the source parameter of memcpy
    void* machineReadableInputPtr;

    //rearrange attribute list to match order of attributes in relation
    //place the attribute data and total attribute length into a record
    int recDataOffset = 0;
    for(int i = 0; i < attrCnt; i++)
    {
        for(int j = 0; j < attrCnt; j++)
        {
            if(0 == strcmp(attrs[i].attrName,attrList[j].attrName))
            {   
		//Switch statement because all input comes in as strings
		//ints and floats need to be converted
		switch (attrs[i].attrType)
		{
		    case 1: //integer
			intBuf = atoi((char*)attrList[j].attrValue);
			machineReadableInputPtr = &intBuf;
			break;

		    case 2: //float
			floatBuf = atof((char*)attrList[j].attrValue);
			machineReadableInputPtr = &floatBuf;
			break;

		    case 0: //string
			machineReadableInputPtr = attrList[j].attrValue;
			break;
		}
                memcpy((char*)outData + recDataOffset, (char*)machineReadableInputPtr, attrs[i].attrLen);
                recDataOffset += attrs[i].attrLen;
            }
        }
    }
    
    //Create InsertFileScan and insert the record
    InsertFileScan ifs(relation, status);
    RID outRid;
    status = ifs.insertRecord(rec, outRid);
    if(status != OK){return status;}
    
// part 6
return OK;

}

