//
//  Transpose rows to columns
//
// Source data:
//   header1, row1Value1, row2Value2
//   header2, row2Value1
//   header3 11/31/2011, row3Value1, row3Value2
//
// Output:
//   header1, header2, header3
//   row1Value1, row2Value1, row3Value1
//   row1Value2, , row3Value2
//
//
//  Created by Dennis Lang on 12/16/18.
// ---
// Copyright (C) 2019 Dennis Lang (landenlabs@gmail.com)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <string>
#include <iosfwd>

#include <vector>
#include <map>
#include <algorithm>
#include <regex>


using namespace std;

// -----------------------------------------------------------------------------------------------
// Split string into parts.
class Split : public std::vector<string>
{
public:
    Split() {
    }
    
    // Examples
    //   Split args(inString, ",", false, 2);
    //   Split args(inString, ",");
    Split(string& str, const char* delimList, bool keepEmpty=true, int maxSplit=numeric_limits<int>::max())
    {
        size_t lastPos = 0;
        size_t pos = str.find_first_of(delimList);
        
        while (pos != string::npos && --maxSplit > 0)
        {
            if (keepEmpty || pos != lastPos)
                push_back(str.substr(lastPos, pos - lastPos));
            lastPos = pos + 1;
            pos = str.find_first_of(delimList, lastPos);
        }
        if (lastPos < str.length())
            push_back(str.substr(lastPos, (maxSplit == 0) ? str.length() : pos - lastPos));
    }
};

// Forward declaration.
void printColumns(const vector<string>& headers,  map<string, Split>& data, int colCnt);

// Helpers
bool contains(const vector<string>& vec, const string& findStr) {
    return std::find(vec.begin(), vec.end(), findStr) != vec.end();
}

template <typename tValue>
bool containsKey(const map<string, tValue>& mapData, const string& findStr) {
    return mapData.find(findStr) != mapData.end();
}

// ===============================================================================================
int main(int argc, const char * argv[]) {
    string              line;
    map<string, Split>  table;      // Rows of fields - to be transposed.
    vector<string>      colHeadings;    // Key (column heading) per row.
    bool                printColHeaders = true;
    int                 colCnt = 0;
    regex               dateRegEx("[0-9]+/[0-9]+/[0-9]+");
    
    if (argc != 2) {
        std::cerr << "Transpose CSV data\n"
            "By Dennis Lang " __DATE__ "\n"
            "Use:   transpose <infile.csv> \n"
            "See:   landenlabs.com\n"
            "    Source data:\n"
            "      header1, row1Value1, row2Value2\n"
            "      header2, row2Value1\n"
            "      header3 11/31/2011, row3Value1, row3Value2\n"
            "      header1, row4Value1, row4Value2\n"
            "      header2, row5Value1\n"
            "      header3 11/31/2011, row6Value1, row6Value2\n"
            "    Output:\n"
            "      header1,header2,header3 ,\n"
            "       row1Value1,  row2Value1 ,  row3Value1,\n"
            "       row2Value2 , ,  row3Value2 ,\n"
            "       row4Value1,  row5Value1 ,  row6Value1,\n"
            "       row4Value2 , ,  row6Value2 ,\n"
            "      header1,header2,header3 ,\n"
            "\n";
        return -1;
    }
    
    // TODO - rewrite using csvtool.h CSV parser https://github.com/landenlabs/csvTool
    ifstream in(argv[1]);
    
    while (getline(in, line)) {
        Split fields(line, ",");
        
        // First field on row is key (becomes column heading)
        // Remove any dates mm/dd/yy
        string rowKey = regex_replace(fields[0], dateRegEx, "");
        
        if (fields.size() > 1) {
            if (!contains(colHeadings, rowKey)) {
                // Build up list of unique row keys (column headings)
                colHeadings.push_back(rowKey);
            }
        }
        
        // When the rows start to repeat (same key), transpose the current table.
        if (containsKey(table, rowKey)) {
            // Dump out data
            if (printColHeaders) {              // Print headers once.
                printColHeaders = false;
                for (auto& col : colHeadings) {
                    cout << col << ",";
                }
                cout << "\n";
            }
            
            printColumns(colHeadings, table, colCnt);
            table.clear();
        }
        
        table.insert( std::pair<string, Split>(rowKey, fields));
        colCnt = max((int)fields.size(), colCnt);
    }
    
    printColumns(colHeadings, table, colCnt);
    
    for (auto& col : colHeadings) {
        cout << col << ",";
    }
    cout << "\n";

    return 0;
}

void printColumns(const vector<string>& headers, map<string, Split>& data, int colCnt) {
    for (int row = 1; row < colCnt; row++) {
        for (const auto& colHeader : headers) {
            auto& colData = data[colHeader];
            if (row < colData.size()) {
                cout << colData[row] << ", ";
            }  else {
                cout << ", ";
            }
        }
        cout << "\n";
    }
}
