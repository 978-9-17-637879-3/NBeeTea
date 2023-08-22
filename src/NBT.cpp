#include <iostream>
#include "NBT.h"
#include "gzip/utils.hpp"
#include "gzip/decompress.hpp"
#include "gzip/compress.hpp"

NBT::NBT(char tagID) {
    this->tagID = tagID;
}

NBT::NBT(char tagID, const std::string &name) {
    this->tagID = tagID;
    this->name = name;
}

void NBT::writeBytes(const char *byteArray, unsigned int &offset, const unsigned int size) {
    valueBytes.insert(valueBytes.end(), byteArray + offset, byteArray + offset + size);
    offset += size;
}

char NBT::getByte() {
    return valueBytes.front();
}

const int SHORT_BYTES = 2;

union SignedShortBytesUnion {
    char bytes[SHORT_BYTES];
    signed short value;
};

signed short NBT::getShort() {
    SignedShortBytesUnion shortBytes;

    for (int i = SHORT_BYTES - 1; i >= 0; i--) {
        shortBytes.bytes[i] = valueBytes[SHORT_BYTES - 1 - i];
    }

    return shortBytes.value;
}

const int INT_BYTES = 4;

union SignedIntBytesUnion {
    char bytes[INT_BYTES];
    signed int value;
};

signed int NBT::getInt() {
    SignedIntBytesUnion intBytes;

    for (int i = INT_BYTES - 1; i >= 0; i--) {
        intBytes.bytes[i] = valueBytes[INT_BYTES - 1 - i];
    }

    return intBytes.value;
}

const int LONG_BYTES = 8;

union SignedLongBytesUnion {
    char bytes[LONG_BYTES];
    signed long value;
};

signed long NBT::getLong() {
    SignedLongBytesUnion longBytes;

    for (int i = LONG_BYTES - 1; i >= 0; i--) {
        longBytes.bytes[i] = valueBytes[LONG_BYTES - 1 - i];
    }

    return longBytes.value;
}

const int FLOAT_BYTES = 4;

union FloatBytesUnion {
    char bytes[FLOAT_BYTES];
    float value;
};

float NBT::getFloat() {
    FloatBytesUnion floatBytes;

    for (int i = FLOAT_BYTES - 1; i >= 0; i--) {
        floatBytes.bytes[i] = valueBytes[FLOAT_BYTES - 1 - i];
    }

    return floatBytes.value;
}

const int DOUBLE_BYTES = 8;

union DoubleBytesUnion {
    char bytes[DOUBLE_BYTES];
    double value;
};

double NBT::getDouble() {
    DoubleBytesUnion doubleBytes;

    for (int i = LONG_BYTES - 1; i >= 0; i--) {
        doubleBytes.bytes[i] = valueBytes[DOUBLE_BYTES - 1 - i];
    }

    return doubleBytes.value;
}

std::vector<char> NBT::getByteVector() {
    return valueBytes;
}

std::string NBT::getString() {
    return std::string(valueBytes.begin(), valueBytes.end());
}

std::vector<signed int> NBT::getIntVector() {
    std::vector<signed int> intVector;
    intVector.resize(valueBytes.size() / (sizeof(signed int)));

    for (int i = 0; i < intVector.size(); i++) {
        SignedIntBytesUnion intBytes;

        for (int j = INT_BYTES - 1; j >= 0; j--) {
            intBytes.bytes[j] = valueBytes[INT_BYTES - 1 - j];
        }

        intVector.push_back(intBytes.value);
    }

    return intVector;
}

std::vector<signed long> NBT::getLongVector() {
    std::vector<signed long> longVector;
    longVector.resize(valueBytes.size() / (sizeof(signed long)));

    for (int i = 0; i < longVector.size(); i++) {
        SignedLongBytesUnion longBytesUnion;

        for (int j = LONG_BYTES - 1; j >= 0; j--) {
            longBytesUnion.bytes[j] = valueBytes[LONG_BYTES - 1 - j];
        }

        longVector.push_back(longBytesUnion.value);
    }

    return longVector;
}

union UnsignedShortBytesUnion {
    char bytes[SHORT_BYTES];
    unsigned short value;
};

char deserializeTagID(const char *byteArray, unsigned int &offset) {
    char tagID = byteArray[offset];
    offset++;
    return tagID;
}

std::string deserializeString(const char *byteArray, unsigned int &offset) {
    UnsignedShortBytesUnion stringLength;

    for (int i = SHORT_BYTES - 1; i >= 0; i--) {
        stringLength.bytes[i] = byteArray[offset + (SHORT_BYTES - 1 - i)];
    }
    offset += SHORT_BYTES;

    std::string str;
    for (int i = 0; i < stringLength.value; i++) {
        str += byteArray[offset + i];
    }
    offset += stringLength.value;
    return str;
}

signed int deserializeSignedInt(const char *byteArray, unsigned int &offset) {
    SignedIntBytesUnion intBytes;

    for (int i = INT_BYTES - 1; i >= 0; i--) {
        intBytes.bytes[i] = byteArray[offset + (INT_BYTES - 1 - i)];
    }
    offset += INT_BYTES;

    return intBytes.value;
}

std::unordered_map<char, int> TAG_BYTE_COUNT_MAP{{TAG_Byte,   1},
                                                 {TAG_Short,  2},
                                                 {TAG_Int,    4},
                                                 {TAG_Long,   8},
                                                 {TAG_Float,  4},
                                                 {TAG_Double, 8}};

void deserializeTagList(NBT *const parentPtr, const char *byteArray, unsigned int &offset);

void deserializeTagCompound(NBT *const parentPtr, const char *byteArray, unsigned int &offset);

void deserializeValue(NBT &childNBT, const char *byteArray, unsigned int &offset) {
    if (TAG_BYTE_COUNT_MAP.count(childNBT.tagID) > 0) {
        childNBT.writeBytes(byteArray, offset, TAG_BYTE_COUNT_MAP[childNBT.tagID]);
    } else if (childNBT.tagID == TAG_List) {
        char listType = deserializeTagID(byteArray, offset);

        childNBT.listType = listType;
        childNBT.childrenCount = deserializeSignedInt(byteArray, offset);

        if (childNBT.childrenCount > 0) {
            deserializeTagList(&childNBT, byteArray, offset);
        }
    } else if (childNBT.tagID == TAG_Compound) {
        deserializeTagCompound(&childNBT, byteArray, offset);
    } else if (childNBT.tagID == TAG_String) {
        UnsignedShortBytesUnion stringLength;

        for (int i = SHORT_BYTES - 1; i >= 0; i--) {
            stringLength.bytes[i] = byteArray[offset + (SHORT_BYTES - 1 - i)];
        }
        offset += SHORT_BYTES;

        childNBT.writeBytes(byteArray, offset, stringLength.value);
    } else if (childNBT.tagID == TAG_Byte_Array) {
        signed int count = deserializeSignedInt(byteArray, offset);
        childNBT.writeBytes(byteArray, offset, count);
    } else if (childNBT.tagID == TAG_Int_Array) {
        signed int count = deserializeSignedInt(byteArray, offset);
        childNBT.writeBytes(byteArray, offset, INT_BYTES * count);
    } else if (childNBT.tagID == TAG_Long_Array) {
        signed int count = deserializeSignedInt(byteArray, offset);
        childNBT.writeBytes(byteArray, offset, LONG_BYTES * count);
    }
}

void deserializeTagList(NBT *const parentPtr, const char *byteArray, unsigned int &offset) {
    assert(parentPtr->tagID == TAG_List);
    for (int c = 0; c < parentPtr->childrenCount; c++) {
        NBT childNBT = NBT(parentPtr->listType);

        deserializeValue(childNBT, byteArray, offset);

        parentPtr->addListChild(childNBT);
    }
}

void deserializeTagCompound(NBT *const parentPtr, const char *byteArray, unsigned int &offset) {
    assert(parentPtr->tagID == TAG_Compound);
    while (byteArray[offset] != TAG_End) {
        char tagID = deserializeTagID(byteArray, offset);
        std::string name = deserializeString(byteArray, offset);

        NBT childNBT = NBT(tagID, name);

        deserializeValue(childNBT, byteArray, offset);

        parentPtr->addCompoundElement(childNBT.name.value(), childNBT);
    }
    offset++;
}

NBT NBT::deserialize(const char *byteArray, const unsigned long byteArraySize) {
    std::string readableNBT;
    if (gzip::is_compressed(byteArray, byteArraySize)) {
        readableNBT = gzip::decompress(byteArray, byteArraySize);
    } else {
        readableNBT = std::string(byteArray, byteArray + byteArraySize);
    }

    const char *readableNBTCharArray = readableNBT.data();

    assert(readableNBTCharArray[0] == TAG_Compound);

    unsigned int offset = 0;

    char tagID = deserializeTagID(readableNBTCharArray, offset);
    std::string name = deserializeString(readableNBTCharArray, offset);

    NBT root = NBT(tagID, name);

    deserializeTagCompound(&root, readableNBTCharArray, offset);

    root.uncompressedBinarySize = offset;

    return root;
}

std::unordered_map<char, std::string> TAG_ID_TO_STRING_MAP{{TAG_Byte,       "TAG_Byte"},
                                                           {TAG_Short,      "TAG_Short"},
                                                           {TAG_Int,        "TAG_Int"},
                                                           {TAG_Long,       "TAG_Long"},
                                                           {TAG_Float,      "TAG_Float"},
                                                           {TAG_Double,     "TAG_Double"},
                                                           {TAG_Byte_Array, "TAG_Byte_Array"},
                                                           {TAG_String,     "TAG_String"},
                                                           {TAG_List,       "TAG_List"},
                                                           {TAG_Compound,   "TAG_Compound"},
                                                           {TAG_Int_Array,  "TAG_Int_Array"},
                                                           {TAG_Long_Array, "TAG_Long_Array"}};

const std::string TAB_STRING = "    ";

void NBT::print(unsigned long depth) {
    for (int i = 0; i < depth; i++) {
        std::cout << TAB_STRING;
    }

    std::cout << TAG_ID_TO_STRING_MAP[tagID];

    if (name.has_value())
        std::cout << " \"" + name.value() + "\"";

    unsigned long subChildrenCount = std::max(listChildren.size(), compoundElements.size());
    if (subChildrenCount > 0) {
        std::cout << " [" + std::to_string(subChildrenCount) + "]" + " {";
        std::cout << std::endl;
        for (NBT child: listChildren) {
            child.print(depth + 1);
        }
        for (std::pair<std::string, NBT> element: compoundElements) {
            element.second.print(depth + 1);
        }

        for (int i = 0; i < depth; i++) {
            std::cout << TAB_STRING;
        }
        std::cout << "}";
    } else {
        std::cout << ": ";

        switch (tagID) {
            case TAG_Byte: {
                std::cout << std::to_string(getByte());
                break;
            }
            case TAG_Short: {
                std::cout << std::to_string(getShort());
                break;
            }
            case TAG_Int: {
                std::cout << std::to_string(getInt());
                break;
            }
            case TAG_Long: {
                std::cout << std::to_string(getLong());
                break;
            }
            case TAG_Float: {
                std::cout << std::to_string(getFloat());
                break;
            }
            case TAG_Double: {
                std::cout << std::to_string(getDouble());
                break;
            }
            case TAG_Byte_Array: {
                std::string joinedArray = "[" + std::to_string(getByteVector().front());
                for (unsigned long i = 1; i < getByteVector().size() - 1; i++) {
                    joinedArray += std::to_string(getByteVector()[i]) + ", ";
                }
                joinedArray += std::to_string(getByteVector().back()) + "]";
                std::cout << joinedArray;
                break;
            }
            case TAG_String: {
                std::cout << getString();
                break;
            }
            case TAG_Int_Array: {
                std::string joinedArray = "[" + std::to_string(getIntVector().front());
                for (unsigned long i = 1; i < getIntVector().size() - 1; i++) {
                    joinedArray += std::to_string(getIntVector()[i]) + ", ";
                }
                joinedArray += std::to_string(getIntVector().back()) + "]";
                std::cout << joinedArray;
                break;
            }
            case TAG_Long_Array: {
                std::string joinedArray = "[" + std::to_string(getLongVector().front());
                for (unsigned long i = 1; i < getLongVector().size() - 1; i++) {
                    joinedArray += std::to_string(getLongVector()[i]) + ", ";
                }
                joinedArray += std::to_string(getLongVector().back()) + "]";
                std::cout << joinedArray;
                break;
            }
            default: {
                std::cout << "UNSUPPORTED TAG ID " << std::to_string(tagID);
                break;
            }
        }
    }


    std::cout << std::endl;
}

void serializeByte(const char &byte, char *byteArray, unsigned int &offset) {
    byteArray[offset++] = byte;
}

void serializeInt(const signed int &n, char *byteArray, unsigned int &offset) {
    SignedIntBytesUnion intBytes;
    intBytes.value = n;
    for (int i = INT_BYTES - 1; i >= 0; i--) {
        serializeByte(intBytes.bytes[i], byteArray, offset);
    }
}

void serializeByteVector(const std::vector<char> &byteVector, char *byteArray, unsigned int &offset) {
    for (const char &byte: byteVector) {
        serializeByte(byte, byteArray, offset);
    }
}

void serializeByteVectorIncludingLength(const std::vector<char> &byteArrayToWrite, char *byteArray, unsigned int &offset) {
    SignedIntBytesUnion countBytes;
    countBytes.value = byteArrayToWrite.size();
    for (int i = INT_BYTES - 1; i >= 0; i--) {
        serializeByte(countBytes.bytes[i], byteArray, offset);
    }

    serializeByteVector(byteArrayToWrite, byteArray, offset);
}

void serializeCString(char *cString, unsigned short cStringLength, char *byteArray, unsigned int &offset) {
    UnsignedShortBytesUnion lengthBytesUnion;
    lengthBytesUnion.value = cStringLength;
    for (int i = SHORT_BYTES - 1; i >= 0; i--) {
        serializeByte(lengthBytesUnion.bytes[i], byteArray, offset);
    }

    for (int i = 0; i < cStringLength; i++) {
        serializeByte(*(cString + i), byteArray, offset);
    }
}

void serializeValue(NBT &nbt, char *byteArray, unsigned int &offset) {
    if (nbt.tagID == TAG_Compound) {
        for (std::pair<std::string, NBT> element: nbt.compoundElements) {
            serializeByte(element.second.tagID, byteArray, offset);
            serializeCString(element.second.name.value().data(), element.second.name.value().size(), byteArray, offset);

            // serialize value
            serializeValue(element.second, byteArray, offset);
        }

        serializeByte(TAG_End, byteArray, offset);
    } else if (nbt.tagID == TAG_List) {
        serializeByte(nbt.listType, byteArray, offset);
        serializeInt(nbt.childrenCount, byteArray, offset);

        for (NBT child: nbt.listChildren) {
            serializeValue(child, byteArray, offset);
        }
    } else if (TAG_BYTE_COUNT_MAP.count(nbt.tagID) > 0) {
        for (const char &byte: nbt.valueBytes) {
            serializeByte(byte, byteArray, offset);
        }
    } else if (nbt.tagID == TAG_String) {
        serializeCString(nbt.valueBytes.data(), nbt.valueBytes.size(), byteArray, offset);
    } else if (nbt.tagID == TAG_Byte_Array || nbt.tagID == TAG_Int_Array || nbt.tagID == TAG_Long_Array) {
        serializeByteVectorIncludingLength(nbt.valueBytes, byteArray, offset);
    }
}

std::vector<char> NBT::serialize(NBT &root, bool compressed) {
    assert(root.uncompressedBinarySize > 0);
    assert(root.tagID == TAG_Compound);

    unsigned int offset = 0;

    std::vector<char> binary;
    binary.resize(root.uncompressedBinarySize);

    serializeByte(root.tagID, binary.data(), offset);
    serializeCString(root.name.value().data(), root.name.value().size(), binary.data(), offset);

    serializeValue(root, binary.data(), offset);

    if (compressed) {
        std::string compressedBinary = gzip::compress(binary.data(), binary.size());

        return std::vector<char>(compressedBinary.begin(), compressedBinary.end());
    }

    return binary;
}