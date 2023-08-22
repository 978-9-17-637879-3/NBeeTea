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
private:
    std::vector<char> valueBytes{}; // empty if TAG_List or TAG_Compound

public:
    NBT *parentPtr;
    char tagID;

    std::vector<NBT> listChildren{};
    char listType{};
    signed int childrenCount{};
    std::unordered_map<std::string, NBT> compoundElements{};

    std::optional<std::string> name;

    void write(const char *byteArray, unsigned int &offset, const unsigned int size);

    void addListChild(const NBT &childNBT) {
        assert(this->tagID == TAG_List);
        listChildren.push_back(childNBT);
    }

    void addCompoundElement(const std::string &childName, const NBT &childNBT) {
        assert(this->tagID == TAG_Compound);
        compoundElements[childName] = childNBT;
    }

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

    void printTree(unsigned long depth = 0);
};

NBT readTree(const char *byteArray);