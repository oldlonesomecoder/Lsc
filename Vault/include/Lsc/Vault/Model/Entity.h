#pragma once

#include <Lsc/Vault/Lib.h>
#include <Lsc/Vault/Model/Field.h>
#include <Lsc/Vault/Model/Table.h>
#include <map>
#include <stack>

namespace Lsc::Vault
{

class Vault;


/*!
 * @brief Entity est la pi&egrave;ce angulaire de la structure de la base de donn&eacute;es entre l'application-usager et Vault.
 *
 */
class VAULT_LIB  Entity
{
    friend class Vault;
    Vault*  mVault = nullptr;
    using Fields = std::vector<Field*>; ///< "Liste" des Fields/colonnes du schema de la DB
    using Cursor = Fields::iterator;    ///< Current position;
    using Stack  = std::stack<std::string>;
    
    Return Parse(const String& Text_);
    Return SetTable(const String& Text_);
    Return ProcessField(const String& Text_);
    
    Entity::Stack mStack;
    
    Field::Collection   mLocalFields; ///< D&eacutesfinition/Structure (Schema) &eacute;tendu ou initiale; Servant &agrave; construire la liste de nouveaux fields.
    Cursor              mCursor;
    Fields              mModel;
    String              mName;
    Table*              mTable = nullptr;
    
public:
    Entity() = default;
    Entity(const Entity&) = default;
    Entity(Entity&&) noexcept = default;
    
    virtual ~Entity();
    
    Entity(std::string Name_, Vault* Vault_);
    Entity(std::string Name_);
    Table* Model()  ; // Maybe null ...
    //--------------------Composition de l'Entity : -------------------------------
    #pragma region EntityCompose
    Table*  CreateTableFromName();
    Entity& operator + (Field&&);
    Entity& operator + (const String&);
    Entity& operator += (const String&);
    Entity& operator += (Field&&);
    Entity& operator << (Field*) noexcept;
    int     operator [](const std::string& Name_);
    
    bool End(Cursor C_);
    Entity::Cursor Begin() { mCursor = mModel.begin(); return mCursor;}
    
    
    std::string  GenerateSchema();
    #pragma endregion EntityCompose
    //-----------------------------------------------------------------------------
    std::string Name() { return mName(); }
//...
};

}
