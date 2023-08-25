#pragma once

#include <vector>
#include <optional>
#include <string>
#include <unordered_map>
#include <cassert>

const char TAG_End = 0x00;
const char TAG_Byte = 0x01;
const char TAG_Short = 0x02;
const char TAG_Int = 0x03;
const char TAG_Long = 0x04;
const char TAG_Float = 0x05;
const char TAG_Double = 0x06;
const char TAG_Byte_Array = 0x07;
const char TAG_String = 0x08;
const char TAG_List = 0x09;
const char TAG_Compound = 0x0A;
const char TAG_Int_Array = 0x0B;
const char TAG_Long_Array = 0x0C;

class NBT {
public:
    char tagID;

    std::vector<NBT> listChildren{};
    char listType{};
    signed int childrenCount{};
    std::unordered_map<std::string, NBT> compoundElements{};

    std::optional<std::string> name;

    std::vector<char> valueBytes{}; // empty if TAG_List or TAG_Compound

    explicit NBT(char tagID);

    NBT addListChild(const NBT &childNBT);

    NBT addCompoundElement(const std::string &childName, NBT childNBT);

    char getByte();

    signed short getShort();

    signed int getInt();

    signed long getLong();

    float getFloat();

    double getDouble();

    std::vector<char> getByteVector();

    std::string getString();

    std::vector<signed int> getIntVector();

    std::vector<signed long> getLongVector();

    void writeBytes(const char *byteArray, unsigned int &offset, const unsigned int size);

    void writeVal(const char &byte);

    void writeVal(const signed short &val);

    void writeVal(const signed int &val);

    void writeVal(const signed long &val);

    void writeVal(const float &val);

    void writeVal(const double &val);

    void writeVal(const std::vector<char> &byteVector);

    void writeVal(const std::string &str);

    void writeVal(const std::vector<int> &intVector);

    void writeVal(const std::vector<long> &longVector);

    NBT(char tagID, const char &byte);
    NBT(char tagID, const signed short &val);
    NBT(char tagID, const signed int &val);
    NBT(char tagID, const signed long &val);
    NBT(char tagID, const float &val);
    NBT(char tagID, const double &val);
    NBT(char tagID, const std::vector<char> &byteVector);
    NBT(char tagID, const std::string &str);
    NBT(char tagID, const std::vector<int> &intVector);
    NBT(char tagID, const std::vector<long> &longVector);


    void print(unsigned long depth = 0);

    static NBT deserialize(const char *byteArray, unsigned long byteArraySize);

    static std::vector<char> serialize(NBT &root, bool compressed = false);
};
