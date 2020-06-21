#ifndef PROPERTY_HPP
#define PROPERTY_HPP
#include <vector>
#include <QString>
#include <QVariant>

#pragma once

#if									\
   defined( linux )				||	\
   defined( __linux )			||	\
   defined( __linux__ )
       #include <memory>
#endif

typedef struct EnumPropertyType {
    int miCurrentIndex{0};
    QStringList mslEnumNames;
} EnumPropertyType;

typedef struct DoublePropertyType {
    double mdValue{0};
    double mdMax{100};
    double mdMin{0};
} DoublePropertyType;

typedef struct IntPropertyType {
    int miValue{0};
    int miMax{100};
    int miMin{0};
} IntPropertyType;

typedef struct UcharPropertyType {
    int mucValue{0}; //UcharPropertyType is always treated as
    int mucMax{255}; //IntPropertyType with QVariant::Int
    int mucMin{0};   //Only cast its member to uchar right before each
} UcharPropertyType; //implementation using static_cast<uchar>(mucVar)

typedef struct FilePathPropertyType {
    QString msFilename;
    QString msFilter;
    QString msMode{"open"};
} FilePathPropertyType;

typedef struct SizePropertyType {
    int miWidth{0};
    int miHeight{0};
} SizePropertyType;

typedef struct PointPropertyType
{ //currently not supported by member functions of superclasses
    int miXposition{0};
    int miYposition{0};
} PointPropertyType;

class Property
{
public:
    Property(const QString & name, const QString & id, int type ) : msName(name), msID(id), miType(type) {}
    virtual ~Property() {}
    QString getName() { return msName; };
    QString getID() { return msID; };
    int getType() { return miType; };
private:
    QString msName;
    QString msID;
    int miType;
};

template < typename T >
class TypedProperty : public Property
{
public:
    TypedProperty( const QString & name, const QString & id, int type, const T & data) : Property(name, id, type), mData(data) {};
    T & getData() { return mData; };
private:
    T mData;
};

typedef std::vector< std::shared_ptr<Property> > PropertyVector;

#endif // PROPERTY_HPP
