#pragma once
#include <Lsc/Vault/Lib.h>
#include <Lsc/Rem/Rem.h>
#include <utility>

namespace Lsc::Vault
{


class Table;

/*!
    @brief  Renferme les informations sur la colonne du schema sqlite3 et du modele.

    
*/
class VAULT_LIB Field
{

DECLOBJ_ID
    
    
    String mName; ///< Nom de la colonne de la table du schema sqlite3 - ET - d'un item du Modele donne par le nom de la classe. ( CLASSNAME_IMPL )
    String mDesc; ///< Description de la colonne.
    Table *mTable = nullptr;
    String mDflt;
    
    String   mFkTableName;
    String   mFkFieldName;

public:
    using SchemaInfoItem = std::pair<const char *, const char *>;
    using SchemaInfo = std::vector<Field::SchemaInfoItem>;
    
    using Collection = std::vector<Field>;
    
    enum class Type : uint8_t
    {
        NUL  = 0,
        TEXT,
        NUMERIC,
        INTEGER,
        REAL,
        RAW,
        CHAR,
        DATE = 100,
        TIME,
        STAMP,
        STRING,
        CURRENCY
    };
    uint32_t mLen = 0;
    /*!
     * @brief Not necessarely exclusive (merged) serial bit selections;
     */
    union Attr
    {
        uint8_t _;
        struct
        {
            uint8_t Primary: 1;
            uint8_t Unique: 1;
            uint8_t Auto: 1;
            uint8_t Null: 1;
            uint8_t Index: 1;
            uint8_t FK: 1;
        };
    }mAttr;
    
    static constexpr uint8_t PK     = 0x1;
    static constexpr uint8_t Unique = 0x2;
    static constexpr uint8_t PKAUTO = 0x5;
    static constexpr uint8_t Null   = 0x8;
    static constexpr uint8_t Index  = 0x10;
    static constexpr uint8_t FK     = 0x20; ///< La table ainsi que la colonne r&eacute;f&eacute;r&eacute;e doivent &ecirc;tre pr&eacute;alablement d&eacuter;finies dans la "Vo&ucirc;te"
    
    
    Field() = default;
    Field(const Field&) = default;
    Field(Field&&) noexcept = default;
    ~Field();

    Field(Table* Table_, std::string Name_);
    Field(std::string Name_, Field::Type, Field::Attr Attr_ = {Field::Null});
    Field(Table *Table_, std::string&& Name_, Field::Type Type_, uint8_t AttrBits_ = 0);
    Field(Table* Table_, const Field::SchemaInfo& SI_);
    
    [[nodiscard]] String Name() const { return mName; }
    
    Field::Attr SetAttributes(uint8_t Attr_);
    Return SetReference(const String& Table_, const String& Field_);
    
    std::string TableName() const;
    String Serialize();
private:
    Field::Type mType = Field::Type::INTEGER;
    int CID=-1;
    
};

}
