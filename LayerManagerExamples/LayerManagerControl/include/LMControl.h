/***************************************************************************
 *
 * Copyright 2012 BMW Car IT GmbH
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

#ifndef __LMCONTROL_H__
#define __LMCONTROL_H__

#include <map>
using std::map;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <string>
using std::string;
/*
 * Functions for printing arrays, vector and maps
 */
void printArray(const char* text, unsigned int* array, int count);

template<typename T>
void printArray(const char* text, T* array, int count);

template<typename T>
void printVector(const char* text, vector<T> v);

template<typename K, typename V>
void printMap(const char* text, std::map<K, V> m);

/*
 * Prints information about the specified screen
 */
void printScreenProperties(unsigned int screenid, const char* prefix = "");

/*
 * Prints information about the specified layer
 */
void printLayerProperties(unsigned int layerid, const char* prefix = "");

/*
 * Prints information about the specified surface
 */
void printSurfaceProperties(unsigned int surfaceid, const char* prefix = "");

/*
 * Prints information about rendered scene
 * (All screens, all rendered layers, all rendered surfaces)
 */
void printScene();

void getCommunicatorPerformance();
void setSurfaceKeyboardFocus(t_ilm_surface surface);
void getKeyboardFocus();
void setSurfaceAcceptsInput(t_ilm_surface surfaceId, string kbdPointerTouch, t_ilm_bool acceptance);
void testNotificationLayer(t_ilm_layer layerid);
void watchLayer(unsigned int* layerids, unsigned int layeridCount);
void watchSurface(unsigned int* surfaceids, unsigned int surfaceidCount);
void setOptimization(t_ilm_uint id, t_ilm_uint mode);
void getOptimization(t_ilm_uint id);
#endif
