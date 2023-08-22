#include "NBT.h"

void NBT::write(const char *byteArray, unsigned int &offset, const unsigned int size) {
    valueBytes.insert(valueBytes.end(), byteArray + offset, byteArray + offset + size);
    offset += size;
}

char NBT::getChar() {
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

std::vector<char> NBT::getBytes() {
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

char readTagIDFromBytes(const char *byteArray, unsigned int &offset) {
    char tagID = byteArray[offset];
    offset++;
    return tagID;
}

std::string readStringFromBytes(const char *byteArray, unsigned int &offset) {
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

signed int readSignedIntFromBytes(const char *byteArray, unsigned int &offset) {
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

void readTagList(NBT *const parentPtr, const char *byteArray, unsigned int &offset);

void readTagCompound(NBT *const parentPtr, const char *byteArray, unsigned int &offset);

void readValue(NBT &childNBT, const char *byteArray, unsigned int &offset) {
    if (TAG_BYTE_COUNT_MAP.count(childNBT.tagID) > 0) {
        childNBT.write(byteArray, offset, TAG_BYTE_COUNT_MAP[childNBT.tagID]);
    } else if (childNBT.tagID == TAG_List) {
        char listType = readTagIDFromBytes(byteArray, offset);

        childNBT.listType = listType;
        childNBT.childrenCount = readSignedIntFromBytes(byteArray, offset);

        if (childNBT.childrenCount > 0) {
            readTagList(&childNBT, byteArray, offset);
        }
    } else if (childNBT.tagID == TAG_Compound) {
        readTagCompound(&childNBT, byteArray, offset);
    } else if (childNBT.tagID == TAG_String) {
        UnsignedShortBytesUnion stringLength;

        for (int i = SHORT_BYTES - 1; i >= 0; i--) {
            stringLength.bytes[i] = byteArray[offset + (SHORT_BYTES - 1 - i)];
        }
        offset += SHORT_BYTES;

        childNBT.write(byteArray, offset, stringLength.value);
    } else if (childNBT.tagID == TAG_Byte_Array) {
        signed int count = readSignedIntFromBytes(byteArray, offset);
        childNBT.write(byteArray, offset, count);
    } else if (childNBT.tagID == TAG_Int_Array) {
        signed int count = readSignedIntFromBytes(byteArray, offset);
        childNBT.write(byteArray, offset, INT_BYTES * count);
    } else if (childNBT.tagID == TAG_Long_Array) {
        signed int count = readSignedIntFromBytes(byteArray, offset);
        childNBT.write(byteArray, offset, LONG_BYTES * count);
    }
}

void readTagList(NBT *const parentPtr, const char *byteArray, unsigned int &offset) {
    assert(parentPtr->tagID == TAG_List);
    for (int c = 0; c < parentPtr->childrenCount; c++) {
        NBT childNBT;

        childNBT.tagID = parentPtr->listType;

        readValue(childNBT, byteArray, offset);

        parentPtr->addListChild(childNBT);
    }
}

void readTagCompound(NBT *const parentPtr, const char *byteArray, unsigned int &offset) {
    while (byteArray[offset] != TAG_End) {
        NBT childNBT;

        childNBT.tagID = readTagIDFromBytes(byteArray, offset);
        childNBT.name = readStringFromBytes(byteArray, offset);

        readValue(childNBT, byteArray, offset);

        parentPtr->addCompoundElement(childNBT.name.value(), childNBT);
    }
    offset++;
}

NBT readTree(const char *byteArray) {
    assert(byteArray[0] == TAG_Compound);

    unsigned int offset = 0;

    NBT root;
    root.tagID = readTagIDFromBytes(byteArray, offset);
    root.name = readStringFromBytes(byteArray, offset);

    readTagCompound(&root, byteArray, offset);

    return root;
}