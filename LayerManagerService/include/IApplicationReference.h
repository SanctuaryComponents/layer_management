
/***************************************************************************
 *
 * Copyright 2010,2011 BMW Car IT GmbH
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef _IAPPLICATION_REFERENCE_H_
#define _IAPPLICATION_REFERENCE_H_
#include <locale>
#include <string.h>
/**
 * Abstract Base Class for all Application References 
 */
class IApplicationReference
{
public:
    /**
     * Constructor: Contructs a Object with the provided Application id and SerialId
     * 
     */
    IApplicationReference(char* applicationUid, unsigned int processId);
    virtual ~IApplicationReference() {}

     virtual char* getApplicationUid();
     static long generateApplicationHash(char* applicationUid);
     virtual long getApplicationHash();
     virtual unsigned int getProcessId();

protected:
    char* m_applicationUid;
    unsigned int m_processId;
};

inline IApplicationReference::IApplicationReference(char* applicationUid, unsigned int processId)
: m_applicationUid(applicationUid)
, m_processId(processId)
{
}

inline char* IApplicationReference::getApplicationUid()
{
    return m_applicationUid;
}

inline unsigned int IApplicationReference::getProcessId()
{
    return m_processId;
}

inline long IApplicationReference::getApplicationHash() 
{
    return generateApplicationHash(m_applicationUid);
}
inline long IApplicationReference::generateApplicationHash(char* applicationUid) 
{
  std::locale loc; 
  const std::collate<char>& colHash = std::use_facet<std::collate<char> >(loc);
  long result = colHash.hash(applicationUid,applicationUid+strlen(applicationUid)); 
  return result;
}
#endif /* _IAPPLICATION_REFERENCE_H_ */
