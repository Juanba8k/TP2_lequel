/**
 * @brief Lequel? language identification based on trigrams
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 *
 * @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a
 * @cite https://builtin.com/machine-learning/cosine-similarity
 */

#include <cmath>
#include <codecvt>
#include <locale>
#include <iostream>
//#include <bits/stdc++.h>
#include "Lequel.h"
#include "CSVData.h"

using namespace std;

/**
 * @brief Builds a trigram profile from a given text.
 *
 * @param text Vector of lines (Text)
 * @return TrigramProfile The trigram profile
 */
TrigramProfile buildTrigramProfile(const Text &text)
{
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    TrigramProfile textProfile;

    int unicodeStringLength;
    wstring unicodeString, unicodeTrigram;
    string trigram;
    
    for (auto line : text)
    {
        if ((line.length() > 0) && (line[line.length() - 1] == '\r'))
                line = line.substr(0, line.length() - 1); //quita el '\r' final 
        
        // Tip: converts UTF-8 string to wstring
        unicodeString = converter.from_bytes(line);
        //wcout << "unicodeString->" << unicodeString << endl;

        unicodeStringLength = unicodeString.length();
        if (unicodeStringLength < 3)
                continue;
        
        std::transform(unicodeString.begin(), unicodeString.end(), unicodeString.begin(), ::tolower); //minusculas
        //wcout << "Minusculas->" << unicodeString << endl;
        for(int i=0; i < unicodeStringLength - 2; i++)
            {
                unicodeTrigram=unicodeString.substr(i,3); //Toma cadenas de 3 caracteres
                // Tip: convert wstring to UTF-8 string
                trigram = converter.to_bytes(unicodeTrigram);
                // cout << "Trigram->(" << trigram << ")" << endl;
                if(textProfile.find(trigram) != textProfile.end()) //time complexity to search the given key in map is O(log n), where n is the number of keys present in map.
                    {
                        //cout << "encontrado->(" << trigram << ")" << endl;
                        textProfile[trigram]+=1.0F;
                    }    
                else
                    {
                        //cout << "Alta de ->(" << trigram << ")"<< endl;
                        textProfile.insert({trigram, 1.0F});
                    }
            }
    }
    return textProfile;
}

/**
 * @brief Normalizes a trigram profile.
 *
 * @param trigramProfile The trigram profile.
 */
void normalizeTrigramProfile(TrigramProfile &trigramProfile)
{
    float sumSquares = 0.F;
    float squareRoot;
    
    //cout << "normalizacion";
    for (auto &element : trigramProfile)
            sumSquares+=(element.second * element.second); // sum of the squares of frequency
    
    squareRoot=sqrtf(sumSquares); //Pythagorean theorem... allways Pythagoras! 
    //printf("\nsuma de cuadrados=%f, raiz=%f\n",sumSquares,squareRoot);
    for (auto &element : trigramProfile)
        {
            element.second /= squareRoot;   //normalized
            //cout << element.first << element.second <<endl;
        }
    return;
}

/**
 * @brief Calculates the cosine similarity between two trigram profiles
 *
 * @param textProfile The text trigram profile
 * @param languageProfile The language trigram profile
 * @return float The cosine similarity score
 */
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile)
{
    string trigram;
    float sumProduct=0.F;
    
    for (auto &elementText : textProfile)
        {
            trigram = elementText.first;
            //cout << "Trigram buscado-> (" << trigram << ")" << endl;
            if(languageProfile.find(trigram) != languageProfile.end()) // is trigram in languageProfile?
                sumProduct+= (elementText.second * languageProfile[trigram]);
        }

    return sumProduct;
}

/**
 * @brief Identifies the language of a text.
 *
 * @param text A Text (vector of lines)
 * @param languages A list of Language objects
 * @return string The language code of the most likely language
 */
string identifyLanguage(const Text &text, LanguageProfiles &languages)
{
    float cosineSimilarity;
    float maxCosineSimilarity=0.F;
    string similarLenguajeCode="---";    
 
    TrigramProfile trigramProfileText=buildTrigramProfile(text);
    
    normalizeTrigramProfile(trigramProfileText);

    for (auto languageProfile: languages)
    {
        cosineSimilarity=getCosineSimilarity(trigramProfileText, languageProfile.trigramProfile);
        cout << "similaridad con " << languageProfile.languageCode << " = " << cosineSimilarity << endl;
        if(cosineSimilarity > maxCosineSimilarity)
            {
                maxCosineSimilarity=cosineSimilarity;
                similarLenguajeCode=languageProfile.languageCode;
            }
    }
    
    if(maxCosineSimilarity > 0)
            similarLenguajeCode+=" ("+to_string(maxCosineSimilarity)+")";
    
    return similarLenguajeCode;
}

/**
 * @brief Obtiene?/genera?/computa? los trigram del file y saves en un CSV file 
 *
 * @param path Path of file to read
 * @return Function succeeded
 */
bool addLanguage(const string path)
{
    Text corpus;
    bool success=false;

    if(getTextFromFile(path, corpus))
        {
            TrigramProfile corpusProfile=buildLanguageProfile(corpus);
            if(!corpusProfile.empty())
            {
                CSVData csvTrigrams;
                string trigram, frecuency;
            
                for (auto &element : corpusProfile)
                {
                    trigram  =element.first;
                    frecuency=to_string((int)element.second);
                    
                    csvTrigrams.push_back({trigram,frecuency});                   
                }
                success=writeCSV("resources/trigrams/gua.csv",csvTrigrams);
            }
        }
    return success;    
}

/**
 * @brief Obtiene?/genera?/computa? los trigram de un corpus 
 *
 * @param corpus Corpus to proccess
 * @return TrigramProfile The corpus profile
 */
TrigramProfile buildLanguageProfile(Text corpus)
{
    return buildTrigramProfile(corpus);
}
