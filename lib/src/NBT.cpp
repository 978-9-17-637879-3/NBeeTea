#include <iostream>
#include "NBT.h"
#include "gzip/utils.hpp"
#include "gzip/decompress.hpp"
#include "gzip/compress.hpp"

NBT::NBT(char tagID) {
    this->tagID = tagID;
}

// return reference to parent to allow for chaining (e.g. a.addListChild(b).addListChild(c))
NBT NBT::addListChild(const NBT &childNBT) {
    assert(this->tagID == TAG_List);

    listChildren.push_back(childNBT);

    return *this;
}

NBT NBT::addCompoundElement(const std::string &childName, NBT childNBT) {
    assert(this->tagID == TAG_Compound);

    if (!childNBT.name.has_value())
        childNBT.name = childName;

    compoundElements.insert(std::make_pair(childName,
                                           childNBT)); // can't use [] operator cuz NBT has no implicit constructor (i think)

    return *this;
}

char NBT::getByte() {
    assert(tagID == TAG_Byte);

    return valueBytes.front();
}

const int SHORT_BYTES = 2;

union SignedShortBytesUnion {
    char bytes[SHORT_BYTES];
    signed short value;
};

signed short NBT::getShort() {
    assert(tagID == TAG_Short);

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
    assert(tagID == TAG_Int);

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
    assert(tagID == TAG_Long);

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
    assert(tagID == TAG_Float);

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
    assert(tagID == TAG_Double);

    DoubleBytesUnion doubleBytes;

    for (int i = LONG_BYTES - 1; i >= 0; i--) {
        doubleBytes.bytes[i] = valueBytes[DOUBLE_BYTES - 1 - i];
    }

    return doubleBytes.value;
}

std::vector<char> NBT::getByteVector() {
    assert(tagID == TAG_Byte_Array);

    return valueBytes;
}

std::string NBT::getString() {
    assert(tagID == TAG_String);

    return std::string(valueBytes.begin(), valueBytes.end());
}

std::vector<signed int> NBT::getIntVector() {
    assert(tagID == TAG_Int_Array);

    std::vector<signed int> intVector;
    int finalSize = (int) (valueBytes.size() / (unsigned long) INT_BYTES);

    for (int i = 0; i < finalSize; i++) {
        SignedIntBytesUnion intBytes;

        for (int j = INT_BYTES - 1; j >= 0; j--) {
            intBytes.bytes[j] = valueBytes[i * INT_BYTES + INT_BYTES - 1 - j];
        }

        intVector.push_back(intBytes.value);
    }

    return intVector;
}

std::vector<signed long> NBT::getLongVector() {
    assert(tagID == TAG_Long_Array);

    std::vector<signed long> longVector;
    int finalSize = (int) (valueBytes.size() / (unsigned long) LONG_BYTES);

    for (int i = 0; i < finalSize; i++) {
        SignedLongBytesUnion longBytesUnion;

        for (int j = LONG_BYTES - 1; j >= 0; j--) {
            longBytesUnion.bytes[j] = valueBytes[i * LONG_BYTES + LONG_BYTES - 1 - j];
        }

        longVector.push_back(longBytesUnion.value);
    }

    return longVector;
}

void NBT::writeBytes(const char *byteArray, unsigned int &offset, const unsigned int size) {
    valueBytes.insert(valueBytes.end(), byteArray + offset, byteArray + offset + size);
    offset += size;
}

// TODO: Implement resizes for potential performance boost

void NBT::writeVal(const char &byte) {
    assert(tagID == TAG_Byte);

//    valueBytes.resize(1);
    valueBytes.push_back(byte);
}

void NBT::writeVal(const short &val) {
    assert(tagID == TAG_Short);

//    valueBytes.resize(SHORT_BYTES);

    SignedShortBytesUnion shortBytes;
    shortBytes.value = val;

    for (int i = SHORT_BYTES - 1; i >= 0; i--) {
        valueBytes.push_back(shortBytes.bytes[i]);
    }
}

void NBT::writeVal(const int &val) {
    assert(tagID == TAG_Int);

//    valueBytes.resize(INT_BYTES);

    SignedIntBytesUnion intBytes;
    intBytes.value = val;

    for (int i = INT_BYTES - 1; i >= 0; i--) {
        valueBytes.push_back(intBytes.bytes[i]);
    }
}

void NBT::writeVal(const long &val) {
    assert(tagID == TAG_Long);

//    valueBytes.resize(LONG_BYTES);

    SignedLongBytesUnion longBytes;
    longBytes.value = val;

    for (int i = LONG_BYTES - 1; i >= 0; i--) {
        valueBytes.push_back(longBytes.bytes[i]);
    }
}

void NBT::writeVal(const float &val) {
    assert(tagID == TAG_Float);

//    valueBytes.resize(FLOAT_BYTES);

    FloatBytesUnion floatBytes;
    floatBytes.value = val;

    for (int i = FLOAT_BYTES - 1; i >= 0; i--) {
        valueBytes.push_back(floatBytes.bytes[i]);
    }
}

void NBT::writeVal(const double &val) {
    assert(tagID == TAG_Double);

//    valueBytes.resize(DOUBLE_BYTES);

    DoubleBytesUnion doubleBytesUnion;
    doubleBytesUnion.value = val;

    for (int i = DOUBLE_BYTES - 1; i >= 0; i--) {
        valueBytes.push_back(doubleBytesUnion.bytes[i]);
    }
}

void NBT::writeVal(const std::vector<char> &byteVector) {
    assert(tagID == TAG_Byte_Array);

    valueBytes = byteVector;
}

void NBT::writeVal(const std::string &str) {
    assert(tagID == TAG_String);

    valueBytes = std::vector<char>(str.begin(), str.end());
}

void NBT::writeVal(const std::vector<int> &intVector) {
    assert(tagID == TAG_Int_Array);

//    valueBytes.resize(valueBytes.size() * INT_BYTES);

    for (const int &val: intVector) {
        SignedIntBytesUnion intBytes;
        intBytes.value = val;

        for (int i = INT_BYTES - 1; i >= 0; i--) {
            valueBytes.push_back(intBytes.bytes[i]);
        }
    }
}

void NBT::writeVal(const std::vector<long> &longVector) {
    assert(tagID == TAG_Long_Array);

//    valueBytes.resize(valueBytes.size() * LONG_BYTES);

    for (const long &val: longVector) {
        SignedLongBytesUnion longBytes;
        longBytes.value = val;

        for (int i = LONG_BYTES - 1; i >= 0; i--) {
            valueBytes.push_back(longBytes.bytes[i]);
        }
    }
}

NBT::NBT(char tagID, const char &byte) : NBT(tagID) {
    this->writeVal(byte);
}

NBT::NBT(char tagID, const short &val) : NBT(tagID) {
    this->writeVal(val);
}

NBT::NBT(char tagID, const int &val) : NBT(tagID) {
    this->writeVal(val);
}

NBT::NBT(char tagID, const long &val) : NBT(tagID) {
    this->writeVal(val);
}

NBT::NBT(char tagID, const float &val) : NBT(tagID) {
    this->writeVal(val);
}

NBT::NBT(char tagID, const double &val) : NBT(tagID) {
    this->writeVal(val);
}

NBT::NBT(char tagID, const std::vector<char> &byteVector) : NBT(tagID) {
    this->writeVal(byteVector);
}

NBT::NBT(char tagID, const std::string &str) : NBT(tagID) {
    this->writeVal(str);
}

NBT::NBT(char tagID, const std::vector<int> &intVector) : NBT(tagID) {
    this->writeVal(intVector);
}

NBT::NBT(char tagID, const std::vector<long> &longVector) : NBT(tagID) {
    this->writeVal(longVector);
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

        NBT childNBT = NBT(tagID);

        deserializeValue(childNBT, byteArray, offset);

        parentPtr->addCompoundElement(name, childNBT);
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

    NBT root = NBT(tagID);
    root.name = name;

    deserializeTagCompound(&root, readableNBTCharArray, offset);

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

void serializeByte(const char &byte, std::vector<char> &serializedBytesVector) {
    serializedBytesVector.push_back(byte);
}

void serializeInt(const signed int &n, std::vector<char> &serializedBytesVector) {
    SignedIntBytesUnion intBytes;
    intBytes.value = n;
    for (int i = INT_BYTES - 1; i >= 0; i--) {
        serializeByte(intBytes.bytes[i], serializedBytesVector);
    }
}

void serializeByteVector(const std::vector<char> &byteVector, std::vector<char> &serializedBytesVector) {
    for (const char &byte: byteVector) {
        serializeByte(byte, serializedBytesVector);
    }
}

void serializeByteVectorIncludingLength(const std::vector<char> &byteArrayToWrite,
                                        std::vector<char> &serializedBytesVector) {
    SignedIntBytesUnion countBytes;
    countBytes.value = byteArrayToWrite.size();
    for (int i = INT_BYTES - 1; i >= 0; i--) {
        serializeByte(countBytes.bytes[i], serializedBytesVector);
    }

    serializeByteVector(byteArrayToWrite, serializedBytesVector);
}

void serializeCString(char *cString, unsigned short cStringLength, std::vector<char> &serializedBytesVector) {
    UnsignedShortBytesUnion lengthBytesUnion;
    lengthBytesUnion.value = cStringLength;
    for (int i = SHORT_BYTES - 1; i >= 0; i--) {
        serializeByte(lengthBytesUnion.bytes[i], serializedBytesVector);
    }

    for (int i = 0; i < cStringLength; i++) {
        serializeByte(*(cString + i), serializedBytesVector);
    }
}

void serializeValue(NBT &nbt, std::vector<char> &serializedBytesVector) {
    if (nbt.tagID == TAG_Compound) {
        for (std::pair<std::string, NBT> element: nbt.compoundElements) {
            serializeByte(element.second.tagID, serializedBytesVector);
            serializeCString(element.second.name.value().data(), element.second.name.value().size(),
                             serializedBytesVector);

            // serialize value
            serializeValue(element.second, serializedBytesVector);
        }

        serializeByte(TAG_End, serializedBytesVector);
    } else if (nbt.tagID == TAG_List) {
        serializeByte(nbt.listType, serializedBytesVector);
        serializeInt(nbt.childrenCount, serializedBytesVector);

        for (NBT child: nbt.listChildren) {
            serializeValue(child, serializedBytesVector);
        }
    } else if (TAG_BYTE_COUNT_MAP.count(nbt.tagID) > 0) {
        for (const char &byte: nbt.valueBytes) {
            serializeByte(byte, serializedBytesVector);
        }
    } else if (nbt.tagID == TAG_String) {
        serializeCString(nbt.valueBytes.data(), nbt.valueBytes.size(), serializedBytesVector);
    } else if (nbt.tagID == TAG_Byte_Array || nbt.tagID == TAG_Int_Array || nbt.tagID == TAG_Long_Array) {
        serializeByteVectorIncludingLength(nbt.valueBytes, serializedBytesVector);
    }
}

std::vector<char> NBT::serialize(NBT &root, bool compressed) {
    assert(root.tagID == TAG_Compound);

    std::vector<char> serializedBytesVector;

    serializeByte(root.tagID, serializedBytesVector);
    if (root.name.has_value())
        serializeCString(root.name.value().data(), root.name.value().size(), serializedBytesVector);
    else {
        std::string emptyStr;
        serializeCString(emptyStr.data(), 0, serializedBytesVector);
    }

    serializeValue(root, serializedBytesVector);

    if (compressed) {
        std::string compressedBinary = gzip::compress(serializedBytesVector.data(), serializedBytesVector.size());

        return std::vector<char>(compressedBinary.begin(), compressedBinary.end());
    }

    return serializedBytesVector;
}
