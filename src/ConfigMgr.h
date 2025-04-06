/*
 * MIT License
 *
 * Copyright (c) 2025 t2dev-circle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * Software.
 */
#ifndef ConfigMgrH
#define ConfigMgrH

#include "util/tc_singleton.h"
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "Public.h"

using namespace tars;

class ConfigMgr : public TC_Singleton<ConfigMgr>
{
public:
    bool init();
public:
    const ConfigData & getConfig() const;
    const std::vector<std::string> & getMainServers() const;
    const std::vector<std::string> & getBackupServers() const;
    
    const bool isTest() const;
private:
    TC_Config m_confFile;
    std::vector<std::string> m_mainServers;
    std::vector<std::string> m_backupServers;
    ConfigData m_confData;
};

#endif // ConfigMgrH
