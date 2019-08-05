//
//  MSON.h
//  snowcrash
//
//  Created by Pavan Kumar Sunkara on 10/9/14.
//  Copyright (c) 2014 Apiary Inc. All rights reserved.
//

#ifndef SNOWCRASH_MSON_H
#define SNOWCRASH_MSON_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include <mpark/variant.hpp>
#include <boost/container/vector.hpp>
#include <stdexcept>

#include "Platform.h"
#include "MarkdownParser.h"
#include "Just.h"

#define ELEMENTS_NOT_SET_ERR std::logic_error("no elements set")

/**
 * MSON Abstract Syntax Tree
 * -------------------------
 *
 * Data types in this document define the MSON AST
 */

namespace mson
{
    /** Markdown */
    typedef mdp::ByteBuffer Markdown;

    /** Literal */
    typedef std::string Literal;

    /**
     * Kind of Base Type
     *
     * This is an internal thing to keep track of what kind of type,
     * that particular named type or member is sub-typed from
     */
    enum BaseType
    {
        UndefinedBaseType = 0,     // Undefined
        PrimitiveBaseType,         // Primitive Types
        ImplicitPrimitiveBaseType, // Primitive Types (implicit)
        ObjectBaseType,            // Object Structure Type
        ImplicitObjectBaseType,    // Object Structure Type (implicit)
        ValueBaseType,             // Array & Enum Structure Type
        ImplicitValueBaseType      // Array & Enum Structure Type (implicit)
    };

    /** Named Types base type table */
    typedef std::map<Literal, BaseType> NamedTypeBaseTable;

    /** Named Types inheritance table */
    typedef std::map<Literal, std::pair<Literal, mdp::BytesRangeSet> > NamedTypeInheritanceTable;

    /** Named Types dependency table */
    typedef std::map<Literal, std::set<Literal> > NamedTypeDependencyTable;

    /** A simple or actual value */
    struct Value {

        /** Constructor */
        Value() : variable(false) {}

        /** Literal value */
        Literal literal;

        /** Flag to denote variable value */
        bool variable;

        /** Check if empty */
        bool empty() const;
    };

    /** Collection of values */
    typedef std::vector<Value> Values;

    /** Type symbol (identifier) */
    struct Symbol {

        /** Constructor */
        Symbol() : variable(false) {}

        /** Name of the symbol */
        Literal literal;

        /** Flag to denote variable type name */
        bool variable;

        /** Check if empty */
        bool empty() const;
    };

    /** Value of type name if based type */
    enum BaseTypeName
    {
        UndefinedTypeName = 0, // Not a base type name
        BooleanTypeName,       // `boolean` type name
        StringTypeName,        // `string` type name
        NumberTypeName,        // `number` type name
        ArrayTypeName,         // `array` type name
        EnumTypeName,          // `enum` type name
        ObjectTypeName         // `object` type name
    };

    /** Base or named type's name */
    struct TypeName {

        /** Constructor */
        TypeName(const BaseTypeName& base_ = UndefinedTypeName) : base(base_) {}

        /** EITHER Base type's value */
        BaseTypeName base;

        /** OR Named type's identifier */
        Symbol symbol;

        /** Check if empty */
        bool empty() const;
    };

    /** Collection of type names */
    typedef std::vector<TypeName> TypeNames;

    /** Attribute of a type */
    enum TypeAttribute
    {
        RequiredTypeAttribute = (1 << 0), // The type is required
        OptionalTypeAttribute = (1 << 1), // The type is optional
        FixedTypeAttribute = (1 << 2),    // The type is fixed
        SampleTypeAttribute = (1 << 3),   // The type is a sample
        DefaultTypeAttribute = (1 << 4),  // The type is default
        NullableTypeAttribute = (1 << 5), // The type is nullable
        FixedTypeTypeAttribute = (1 << 6) // This type works like fixed, but it is unheritable
    };

    /** List of type attributes */
    typedef unsigned int TypeAttributes;

    /** Defines sub-typed types for a type */
    struct TypeSpecification {

        /** Name of the type */
        TypeName name;

        /** Array of nested types */
        TypeNames nestedTypes;

        /** Check if empty */
        bool empty() const;
    };

    /** Definition of an instance of a type */
    struct TypeDefinition {

        /** Constructor */
        TypeDefinition() : baseType(UndefinedBaseType), attributes(0) {}

        /**
         * Base Type (for the type definition)
         *
         * Representing the base type from which this member or
         * named type is sub-typed from. Not present in the AST
         */
        BaseType baseType;

        /** Type specification */
        TypeSpecification typeSpecification;

        /** List of type attributes (byte-wise OR) */
        TypeAttributes attributes;

        /** Check if empty */
        bool empty() const;
    };

    /** Value definition of a type instance */
    struct ValueDefinition {

        /** List of values */
        Values values = {};

        /** Type of the values */
        TypeDefinition typeDefinition = {};

        /** Check if empty */
        bool empty() const;
    };

    /** Forward Declaration for MemberType */
    struct MemberType;

    /** Collection of elements */
    typedef boost::container::vector<MemberType> MemberTypes;

    // TODO OPTIM @tjanc@ get rid of this
    struct Example {
    private:
        using Content = mpark::variant<Literal, MemberTypes>;
        Content content;

    public:
        explicit Example(Literal value) noexcept : content{ std::move(value) } {}
        explicit Example(MemberTypes members) noexcept : content{ std::move(members) } {}

    public:
        const Literal* literal() const noexcept
        {
            return mpark::get_if<Literal>(&content);
        }

        const MemberTypes* members() const noexcept
        {
            return mpark::get_if<MemberTypes>(&content);
        }
    };

    /** MSON Type Section */
    struct TypeSection {
        using Empty = mpark::monostate;
        using BlockDescription = Markdown;
        using MemberTypeGroup = just<MemberTypes, struct member_type_group_tag>;
        using SampleSection = just<Example, struct sample_tag>;
        using DefaultSection = just<Example, struct default_tag>;

    private:
        using Content = mpark::variant< //
            Empty,
            BlockDescription,
            MemberTypeGroup,
            SampleSection,
            DefaultSection>;

        Content content;
        BaseType base_;

    public:
        constexpr TypeSection() noexcept : content{}, base_{ UndefinedBaseType } {}

    public:
        explicit TypeSection(BlockDescription c, BaseType b = UndefinedBaseType) noexcept : content{ std::move(c) },
                                                                                            base_{ b }
        {
        }

        explicit TypeSection(MemberTypeGroup c, BaseType b = UndefinedBaseType) noexcept : content{ std::move(c) },
                                                                                           base_{ b }
        {
        }

        explicit TypeSection(SampleSection c, BaseType b = UndefinedBaseType) noexcept : content{ std::move(c) },
                                                                                         base_{ b }
        {
        }

        explicit TypeSection(DefaultSection c, BaseType b = UndefinedBaseType) noexcept : content{ std::move(c) },
                                                                                          base_{ b }
        {
        }

    public:
        TypeSection& operator=(BlockDescription c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        TypeSection& operator=(MemberTypeGroup c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        TypeSection& operator=(SampleSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        TypeSection& operator=(DefaultSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

    public:
        /** EITHER Block Description */
        const BlockDescription* description() const noexcept
        {
            return mpark::get_if<BlockDescription>(&content);
        }
        BlockDescription* description() noexcept
        {
            return mpark::get_if<BlockDescription>(&content);
        }

        /** OR Member Type Group */
        const MemberTypeGroup* members() const noexcept
        {
            return mpark::get_if<MemberTypeGroup>(&content);
        }
        MemberTypeGroup* members() noexcept
        {
            return mpark::get_if<MemberTypeGroup>(&content);
        }

        /** OR SampleSection */
        const SampleSection* sample() const noexcept
        {
            return mpark::get_if<SampleSection>(&content);
        }
        SampleSection* sample() noexcept
        {
            return mpark::get_if<SampleSection>(&content);
        }

        /** OR DefaultSection */
        const DefaultSection* dfault() const noexcept
        {
            return mpark::get_if<DefaultSection>(&content);
        }
        DefaultSection* dfault() noexcept
        {
            return mpark::get_if<DefaultSection>(&content);
        }

    public:
        constexpr bool empty() const noexcept
        {
            return nullptr != mpark::get_if<Empty>(&content);
        }

        constexpr BaseType base() const noexcept
        {
            return base_;
        }
    };

    /** Collection of type sections */
    typedef boost::container::vector<TypeSection> TypeSections;

    /** User-define named type */
    struct NamedType {

        /** Name of the type */
        TypeName name;

        /** The ancestor type definition */
        TypeDefinition typeDefinition;

        /** List of named type's sections */
        TypeSections sections;

        /** Check if empty */
        bool empty() const;
    };

    /** Individual member of an array or enum structure */
    struct ValueMember {

        /** Inline description */
        Markdown description = {};

        /** Definition of member's value */
        ValueDefinition valueDefinition = {};

        /** List of member type's sections */
        TypeSections sections = {};

        /** Check if empty */
        bool empty() const;
    };

    /** Name of a property member */
    struct PropertyName {

        /** EITHER Literal name of the property */
        Literal literal;

        /** OR Variable name of the property */
        ValueDefinition variable;

        /** Check if empty */
        bool empty() const;
    };

    /** Individual member of an object structure */
    struct PropertyMember : public ValueMember {

        /** Name of the property */
        PropertyName name;

        /** Check if empty */
        bool empty() const;
    };

    /** Mixin type */
    typedef TypeDefinition Mixin;

    /** One Of type */
    typedef MemberTypes OneOf;

    /** MSON Member Type */
    struct MemberType {

        using Empty = mpark::monostate;
        using PropertyMemberSection = PropertyMember;
        using ValueMemberSection = ValueMember;
        using MixinSection = Mixin;
        using OneOfSection = just<MemberTypes, struct one_of_tag>;
        using GroupSection = just<MemberTypes, struct group_tag>;

    private:
        using Content = mpark::variant< //
            Empty,                      // OPTIM @tjanc@ remove Empty
            PropertyMemberSection,
            ValueMemberSection,
            MixinSection,
            OneOfSection,
            GroupSection>;

        Content content = {};

    public:
        constexpr MemberType() noexcept : content{} {}

        /** EITHER Property member */
        const PropertyMember* property() const noexcept
        {
            return mpark::get_if<PropertyMemberSection>(&content);
        }

        /** OR Value member */
        const ValueMember* value() const noexcept
        {
            return mpark::get_if<ValueMemberSection>(&content);
        }

        /** OR Mixin member */
        const Mixin* mixin() const noexcept
        {
            return mpark::get_if<MixinSection>(&content);
        }

        /** OR One of member */
        const OneOf* oneOf() const noexcept
        {
            auto* result = mpark::get_if<OneOfSection>(&content);
            return (result ? &**result : nullptr);
        }

        /** OR Collection of MemberTypes */
        const MemberTypes* group() const noexcept
        {
            auto result = mpark::get_if<GroupSection>(&content);
            return result ? &**result : nullptr;
        }

        /** Functions which allow the building of member type */
        explicit MemberType(PropertyMemberSection&& c) noexcept : content{ std::move(c) } {}
        explicit MemberType(ValueMemberSection&& c) noexcept : content{ std::move(c) } {}
        explicit MemberType(MixinSection&& c) noexcept : content{ std::move(c) } {}
        explicit MemberType(OneOfSection&& c) noexcept : content{ std::move(c) } {}
        explicit MemberType(GroupSection&& c) noexcept : content{ std::move(c) } {}

        MemberType& operator=(PropertyMemberSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        MemberType& operator=(ValueMemberSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        MemberType& operator=(MixinSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        MemberType& operator=(OneOfSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        MemberType& operator=(GroupSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        bool empty() const noexcept
        {
            return nullptr != mpark::get_if<Empty>(&content);
        }

        template <typename F>
        void visit(F&& f)
        {
            mpark::visit(std::forward<F>(f), content);
        }

        template <typename F>
        void visit(F&& f) const
        {
            mpark::visit(std::forward<F>(f), content);
        }
    };
}

#endif
