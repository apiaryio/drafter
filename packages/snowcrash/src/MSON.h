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

    /** Forward Declaration for element */
    struct Element;

    /** Collection of elements */
    typedef boost::container::vector<Element> Elements;

    /** Section of a type */
    struct TypeSection {

        /** Class of a type section */
        enum Class
        {
            UndefinedClass = 0,    // Unknown
            BlockDescriptionClass, // Markdown block description
            MemberTypeClass,       // Contains member types
            SampleClass,           // Sample value(s) for member types
            DefaultClass           // Default value(s) for member types
        };

        /** Content of the type section */
        struct Content {

            /** EITHER Block description */
            Markdown description;

            /** OR Literal value */
            Literal value;

            /** OR Collection of elements */
            Elements& elements();
            const Elements& elements() const;

            /** Constructor */
            Content(const Markdown& description_ = Markdown(), const Literal& value_ = Literal());

            /** Copy constructor */
            Content(const TypeSection::Content& rhs);

            /** Assignment operator */
            TypeSection::Content& operator=(const TypeSection::Content& rhs);

            /** Desctructor */
            ~Content();

        private:
            std::unique_ptr<Elements> m_elements;
        };

        /** Constructor */
        TypeSection(const TypeSection::Class& klass_ = TypeSection::UndefinedClass)
            : baseType(UndefinedBaseType), klass(klass_)
        {
        }

        /** Base Type (for the parent of the type section) */
        BaseType baseType;

        /** Denotes the class of the type section */
        TypeSection::Class klass;

        /** Content of the type section */
        TypeSection::Content content;

        /** Check if empty */
        bool empty() const;
    };

    /** Collection of type sections */
    typedef std::vector<TypeSection> TypeSections;

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
    typedef Elements OneOf;

    /** Element of a type section */
    struct Element {

        using Empty = mpark::monostate;
        using PropertyMemberSection = PropertyMember;
        using ValueMemberSection = ValueMember;
        using MixinSection = Mixin;
        using OneOfSection = just<Elements, struct one_of_tag>;
        using GroupSection = just<Elements, struct group_tag>;

    private:
        using Content = mpark::variant< //
            Empty,
            PropertyMemberSection,
            ValueMemberSection,
            MixinSection,
            OneOfSection,
            GroupSection>;

        Content content = {};

    public:
        constexpr Element() noexcept : content{} {}

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

        /** OR Collection of elements */
        const Elements* group() const noexcept
        {
            auto result = mpark::get_if<GroupSection>(&content);
            return result ? &**result : nullptr;
        }

        /** Functions which allow the building of member type */
        explicit Element(PropertyMemberSection&& c) noexcept : content{ std::move(c) } {}
        explicit Element(ValueMemberSection&& c) noexcept : content{ std::move(c) } {}
        explicit Element(MixinSection&& c) noexcept : content{ std::move(c) } {}
        explicit Element(OneOfSection&& c) noexcept : content{ std::move(c) } {}
        explicit Element(GroupSection&& c) noexcept : content{ std::move(c) } {}

        Element& operator=(PropertyMemberSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        Element& operator=(ValueMemberSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        Element& operator=(MixinSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        Element& operator=(OneOfSection c) noexcept
        {
            content = std::move(c);
            return *this;
        }

        Element& operator=(GroupSection c) noexcept
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
