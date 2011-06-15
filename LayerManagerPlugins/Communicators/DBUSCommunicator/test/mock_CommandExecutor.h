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

#ifndef MockCommandExecutor_H_
#define MockCommandExecutor_H_

#include "GraphicalSurface.h"
#include "ICommand.h"
#include "ICommandExecutor.h"
#include <list>
#include <gmock/gmock.h>  // Brings in Google Mock.
class MockCommandExecutor : public ICommandExecutor {
 public:
  MOCK_METHOD1(execute, bool(ICommand* commandToBeExecuted));

  MOCK_METHOD3(startManagement, bool(const int width, const int height, const char* displayName));
  MOCK_METHOD0(stopManagement, bool());

  MOCK_METHOD0(getScene, Scene*());

  MOCK_METHOD0(getRendererList, RendererList*());
  MOCK_METHOD1(addRenderer, void(IRenderer* renderer));
  MOCK_METHOD1(removeRenderer, void(IRenderer* renderer));

  MOCK_METHOD0(getCommunicatorList, CommunicatorList*());
  MOCK_METHOD1(addCommunicator, void(ICommunicator* renderer));
  MOCK_METHOD1(removeCommunicator, void(ICommunicator* renderer));

  MOCK_CONST_METHOD1(getLayerTypeCapabilities, uint(LayerType));
  MOCK_CONST_METHOD1(getNumberOfHardwareLayers, uint(uint));
  MOCK_CONST_METHOD1(getScreenResolution, uint*(uint));
  MOCK_CONST_METHOD1(getScreenIDs, uint*(uint*));
};

#endif /* MockCommandExecutor_H_ */
