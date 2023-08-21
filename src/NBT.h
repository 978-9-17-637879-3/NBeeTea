#pragma once

#include <vector>
#include <optional>
#include <string>
#include <map>
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

    std::vector<NBT> listChildren;
    char listType;
    signed int listLength;
    std::unordered_map<std::string, NBT> compoundElements;

    std::optional<std::string> name;

    void write(const char *byteArray, unsigned int &offset, const unsigned int size);

    void addListChild(const NBT &childNBT) {
        assert(this->tagID == TAG_List);
        listChildren.push_back(childNBT);
    }

    void addCompoundElement(const std::string &childName, const NBT &childNBT) {
        assert(this->tagID == TAG_Compound);
        compoundElements.insert(std::make_pair(childName, childNBT));
    }

    char getChar();

    signed short getShort();

    signed int getInt();

    signed long getLong();

    float getFloat();

    double getDouble();

    std::vector<char> getBytes();

    std::string getString();

    std::vector<signed int> getIntVector();

    std::vector<signed long> getLongVector();


};

NBT readTree(const char *byteArray, const unsigned long &byteArrayLength);

//class NBTTree {
//public:
//    NBTTree *parentPtr;
//    NBT nbt;
//    std::vector<NBTTree> listChildren;
//    std::unordered_map<std::string, NBTTree> compoundElements;
//
//    // Root of tree
//    NBTTree(NBT nbt) {
//        this->nbt = nbt;
//    }
//
//    // Branch of tree
//    NBTTree(NBTTree *parentPtr, NBT nbt) {
//        this->parentPtr = parentPtr;
//        this->nbt = nbt;
//    }
//
//    void addListChild(NBT childNBT) {
//        assert(nbt.tagID == TAG_List);
//        NBTTree child = NBTTree(this, childNBT);
//        listChildren.push_back(child);
//    }
//
//    void addCompoundElement(std::string name, NBT childNBT) {
//        assert(nbt.tagID == TAG_Compound);
//        NBTTree child = NBTTree(this, childNBT);
//        compoundElements.insert(std::make_pair(name, child));
//    }
//};

