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

#ifndef _ICOMMUNICATOR_H_
#define _ICOMMUNICATOR_H_

class ICommandExecutor;

class ICommunicator
{
public:
    /**
     * constructor: any communicator need a executor for commands
     * @param executor Pointer to an object to send commands to
     */
    ICommunicator(ICommandExecutor* executor);

    /**
     * Start communication process, ie start specific listening process of communication method
     */
    virtual bool start() = 0;

    /**
     * Stop communication. Stop sending command objects.
     */
    virtual void stop() = 0;

    /**
     * Process communication. 
     */
    virtual void process() = 0;

    /**
     * Switch debug mode of this component on or off
     * @param onoff Turn on debug mode (true) or off (false)
     */
    virtual void setdebug(bool onoff) = 0;

protected:
    ICommandExecutor* m_executor;
};

inline ICommunicator::ICommunicator(ICommandExecutor* executor)
: m_executor(executor)
{
}


#endif // _ICOMMUNICATOR_H_
