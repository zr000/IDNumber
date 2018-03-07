/*
 * Copyright (C) 2016-2017, Egor Pugin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bazel.h"

#include <algorithm>

#include "driver.h"

namespace {

void trimQuotes(std::string &s)
{
    if (s.empty())
        return;
    if (s.front() == '\"')
        s = s.substr(1);
    if (s.empty())
        return;
    if (s.back() == '\"')
        s = s.substr(0, s.size() - 1);

    //s = s.substr(s.find_first_not_of("\""));
    //s = s.substr(0, s.find_last_not_of("\"") + 1);
}

std::string prepare_project_name(const std::string &s)
{
    std::string t = s;
    std::replace(t.begin(), t.end(), '-', '_');
    return t;
}

}

namespace bazel
{

void Parameter::trimQuotes()
{
    ::trimQuotes(name);
    Values vls;
    for (auto &v : values)
    {
        auto s = v;
        ::trimQuotes(s);
        vls.insert(s);
    }
    values = vls;
}

void Function::trimQuotes()
{
    ::trimQuotes(name);
    for (auto &p : parameters)
        p.trimQuotes();
}

void File::trimQuotes()
{
    for (auto &f : functions)
        f.trimQuotes();
}

Values File::getFiles(const Name &name)
{
    Values values;
    for (auto &f : functions)
    {
        if (!(f.name == "cc_library" || f.name == "cc_binary"))
            continue;

        auto i = std::find_if(f.parameters.begin(), f.parameters.end(), [](const auto &p)
        {
            return "name" == p.name;
        });
        if (i == f.parameters.end() || i->values.empty() || prepare_project_name(*i->values.begin()) != name)
            continue;

        i = std::find_if(f.parameters.begin(), f.parameters.end(), [](const auto &p)
        {
            return "hdrs" == p.name;
        });
        if (i != f.parameters.end())
        {
            values.insert(i->values.begin(), i->values.end());
        }

        i = std::find_if(f.parameters.begin(), f.parameters.end(), [](const auto &p)
        {
            return "srcs" == p.name;
        });
        if (i != f.parameters.end())
        {
            values.insert(i->values.begin(), i->values.end());
        }
    }
    return values;
}

File parse(const std::string &s)
{
    BazelParserDriver pd;
    pd.parse(s);
    pd.bazel_file.trimQuotes();
    return pd.bazel_file;
}

} // namespace bazel
