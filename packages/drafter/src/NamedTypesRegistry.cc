#include "NamedTypesRegistry.h"

#include <algorithm>
#include <map>
#include <set>
#include <string>

#include "Blueprint.h"
#include "ConversionContext.h"
#include "MSON.h"
#include "NodeInfo.h"
#include "RefractDataStructure.h"
#include "RefractElementFactory.h"
#include "refract/Exception.h"
#include "refract/Registry.h"
#include "refract/InfoElements.h"
#include "refract/Element.h"

#undef DEBUG_DEPENDENCIES

#ifdef DEBUG_DEPENDENCIES
#include <iostream>
#endif /* DEBUG_DEPENDENCIES */

using namespace drafter;
using namespace refract;

namespace
{
    typedef std::vector<NodeInfo<snowcrash::DataStructure> > DataStructures;

    void FindNamedTypes(NodeInfoCollection<snowcrash::Elements>& elements, DataStructures& found)
    {

        for (NodeInfoCollection<snowcrash::Elements>::const_iterator i = elements.begin(); i != elements.end(); ++i) {

            if (i->node->element == snowcrash::Element::DataStructureElement) {
                found.push_back(MakeNodeInfo(i->node->content.dataStructure, i->sourceMap->content.dataStructure));
            } else if (!i->node->content.resource.attributes.empty()) {
                found.push_back(
                    MakeNodeInfo(i->node->content.resource.attributes, i->sourceMap->content.resource.attributes));
            } else if (i->node->element == snowcrash::Element::CategoryElement) {
                NodeInfoCollection<snowcrash::Elements> children(
                    MakeNodeInfo(i->node->content.elements(), i->sourceMap->content.elements()));
                FindNamedTypes(children, found);
            }
        }
    }
}

namespace
{

    const std::string& name(const mson::TypeName& tn)
    {
        return tn.symbol.literal;
    }

    const std::string& name(const snowcrash::DataStructure* ds)
    {
        return ds->name.symbol.literal;
    }

    const std::string& name(const mson::TypeSpecification& ts)
    {
        return ts.name.symbol.literal;
    }

    const std::string& name(const mson::ValueDefinition& vd)
    {
        return name(vd.typeDefinition.typeSpecification);
    }

    const std::string& name(const mson::ValueMember& vm)
    {
        return name(vm.valueDefinition);
    }

    const std::string& name(const mson::PropertyMember& pm)
    {
        return name(pm.valueDefinition);
    }

    const std::string& name(const mson::Mixin& mx)
    {
        return name(mx.typeSpecification);
    }
}

namespace
{

    using Members = std::set<std::string>;
    using MembersMap = std::map<std::string, Members>;

    Members collectMembers(const mson::Elements& elements);
    Members collectMembers(const mson::TypeSections& ts);
    Members collectMembers(const mson::TypeNames& tn);

    void collectElementMember(const mson::Element::PropertyMemberSection& el, Members& result)
    {
        std::string member;
        const mson::TypeSections* ts = NULL;
        const mson::TypeNames* tn = NULL;

        if (!name(el).empty()) {
            member = name(el);
        } else if (!name(el.name.variable).empty()) {
            member = name(el.name.variable);
        } else if (!el.valueDefinition.typeDefinition.typeSpecification.nestedTypes.empty()) {
            // - name (array|enum[<nested>, <type>])
            tn = &el.valueDefinition.typeDefinition.typeSpecification.nestedTypes;
        } else {
            ts = &el.sections;
        }

        if (!member.empty()) {
            result.insert(member);
        } else if (ts) {
            Members sub = collectMembers(*ts);
            result.insert(sub.begin(), sub.end());
        } else if (tn) {
            Members sub = collectMembers(*tn);
            result.insert(sub.begin(), sub.end());
        }
    }

    void collectElementMember(const mson::Element::ValueMemberSection& el, Members& result)
    {
        std::string member;
        const mson::TypeSections* ts = NULL;

        if (!name(el).empty()) {
            member = name(el);
        } else {
            ts = &el.sections;
        }

        if (!member.empty()) {
            result.insert(member);
        } else if (ts) {
            Members sub = collectMembers(*ts);
            result.insert(sub.begin(), sub.end());
        }
    }

    void collectElementMember(const mson::Element::MixinSection& el, Members& result)
    {
        std::string member = name(el);

        if (!member.empty()) {
            result.insert(std::move(member));
        }
    }

    void collectElementMember(const mson::Element::OneOfSection& el, Members& result) {}

    void collectElementMember(const mson::Element::GroupSection& el, Members& result) {}

    void collectElementMember(const mson::Element::Empty& el, Members& result) {}

    struct CollectElementMemberLambda {
        Members& members;
        template <typename Section>
        void operator()(const Section& s)
        {
            collectElementMember(s, members);
        }
    };

    Members collectMembers(const mson::Elements& elements)
    {
        Members members;

        for (const auto& el : elements)
            el.visit(CollectElementMemberLambda{ members });

        return members;
    }

    Members collectMembers(const mson::TypeSections& ts)
    {
        Members members;

        // map direct members
        for (mson::TypeSections::const_iterator its = ts.begin(); its != ts.end(); ++its) {
            Members sub = collectMembers(its->content.elements());
            members.insert(sub.begin(), sub.end());
        }

        return members;
    }

    Members collectMembers(const mson::TypeNames& tn)
    {
        Members members;

        for (mson::TypeNames::const_iterator itn = tn.begin(); itn != tn.end(); ++itn) {
            members.insert(name(*itn));
        }

        return members;
    }

    Members collectMembers(const snowcrash::DataStructure* ds)
    {
        return collectMembers(ds->sections);
    }

    Members collectMembers(const std::string& id, const MembersMap& members, Members& resolved)
    {
        Members collected;

        if (resolved.find(id) != resolved.end()) {
            return collected;
        }

        resolved.insert(id);

        MembersMap::const_iterator member = members.find(id);
        if (member == members.end()) {
            return collected;
        }

        for (Members::const_iterator i = member->second.begin(); i != member->second.end(); ++i) {
            Members sub = collectMembers(*i, members, resolved);
            collected.insert(*i);
            collected.insert(sub.begin(), sub.end());
        }

        return collected;
    }

    struct DependencyTypeInfo {

        typedef std::map<std::string, std::string> InheritanceMap;
        InheritanceMap childToParent;

        MembersMap objectToMembers;

        typedef std::map<std::string, const snowcrash::DataStructure*> ElementMap;
        ElementMap nameToElement;

        const std::string& parent(const snowcrash::DataStructure* ds) const
        {
            return ds->typeDefinition.typeSpecification.name.symbol.literal;
        }

        bool hasParent(const snowcrash::DataStructure* ds) const
        {
            return !parent(ds).empty();
        }

        DependencyTypeInfo(const DataStructures& elements)
        {

            // map inheritance && element
            for (DataStructures::const_iterator i = elements.begin(); i != elements.end(); ++i) {

                nameToElement[name(i->node)] = &(*i->node);

                if (hasParent(i->node)) {
                    childToParent[name(i->node)] = parent(i->node);

#ifdef DEBUG_DEPENDENCIES
                    std::cout << "Parent: " << name(i->node) << "=>" << parent(i->node) << std::endl;
#endif
                }
            }

            // Map members
            MembersMap members;
            for (DataStructures::const_iterator i = elements.begin(); i != elements.end(); ++i) {
                members[name(i->node)] = collectMembers(i->node);
            }

            // Map members into deep
            for (MembersMap::const_iterator i = members.begin(); i != members.end(); ++i) {
                Members resolved;
                objectToMembers[i->first] = collectMembers(i->first, members, resolved);
            }

#ifdef DEBUG_DEPENDENCIES
            // debug out members
            for (MembersMap::const_iterator i = objectToMembers.begin(); i != objectToMembers.end(); ++i) {
                std::cout << "Members: " << i->first << std::endl;
                for (Members::const_iterator it = objectToMembers[i->first].begin();
                     it != objectToMembers[i->first].end();
                     ++it) {
                    std::cout << " - " << *it << std::endl;
                }
            }
#endif /* DEBUG_DEPENDENCIES */
        }

        bool hasMember(const snowcrash::DataStructure* object, const snowcrash::DataStructure* member) const
        {
            MembersMap::const_iterator members = objectToMembers.find(name(object));
            if (members == objectToMembers.end()) {
                return false;
            }

            return members->second.find(name(member)) != members->second.end();
        }

        bool hasAncestor(const snowcrash::DataStructure* object, const snowcrash::DataStructure* ancestor) const
        {
            std::vector<std::string> symbols;

            const std::string& object_typename = name(object);
            const std::string& ancestor_typename = name(ancestor);

            auto current_typename = object_typename;

            while (true) {
                if (current_typename == ancestor_typename)
                    return true; // found ancestor (happy path)
                auto next_typename = childToParent.find(current_typename);
                if (next_typename == childToParent.end())
                    return false; // arrived at the end

                if (symbols.end() != std::find(symbols.begin(), symbols.end(), current_typename))
                    return false; // circular reference
                symbols.emplace_back(current_typename);
                current_typename = next_typename->second;
            }

            return false;
        }

        mson::BaseTypeName GetType(const snowcrash::DataStructure* ds) const
        {
            return ds->typeDefinition.typeSpecification.name.base;
        }

        mson::BaseTypeName ResolveType(const snowcrash::DataStructure* object) const
        {
            std::string s = name(object);

            while (!s.empty()) {
                ElementMap::const_iterator ei = nameToElement.find(s);
                if (ei == nameToElement.end()) {
                    return mson::UndefinedTypeName;
                }

                mson::BaseTypeName type = GetType(ei->second);

                if (type != mson::UndefinedTypeName) {
                    return type;
                }

                InheritanceMap::const_iterator i = childToParent.find(s);
                if (i == childToParent.end() || (s == i->second)) {
                    return mson::UndefinedTypeName;
                }

                s = i->second;
            }

            return mson::UndefinedTypeName;
        }
    };

    struct InheritanceComparator {

        const DependencyTypeInfo& typeInfo;

        InheritanceComparator(const DependencyTypeInfo& typeInfo) : typeInfo(typeInfo) {}

        /* This is a comparator for std::sort so it has to compare
         * objects in strictly weak ordering otherwise it would crash
         * with sort going out of container bounds, see
         * http://stackoverflow.com/questions/24048022/what-causes-stdsort-to-access-address-out-of-range
         * for more details.
         * The order is following, if F has S as an ancestor or a member
         * then S < F and conversely, if none of this applies we compare objects by
         * names to provide some ordering for all objects as we don't
         * really care.
         */
        bool operator()(DataStructures::const_reference first, DataStructures::const_reference second)
        {

            if (typeInfo.hasAncestor(first.node, second.node) || typeInfo.hasMember(first.node, second.node)) {
                return false;
            }

            if (typeInfo.hasAncestor(second.node, first.node) || typeInfo.hasMember(second.node, first.node)) {
                return true;
            }

            return name(first.node) < name(second.node);
        }
    };

}

namespace drafter
{

    void RegisterNamedTypes(const NodeInfo<snowcrash::Elements>& elements, ConversionContext& context)
    {
        DataStructures found;
        NodeInfoCollection<snowcrash::Elements> elementCollection(elements);

        FindNamedTypes(elementCollection, found);

#ifdef DEBUG_DEPENDENCIES
        std::cout << "==DEPENDENCIES INFO BEGIN==" << std::endl;
#endif /* DEBUG_DEPENDENCIES */

        DependencyTypeInfo typeInfo(found);

        std::sort(found.begin(), found.end(), InheritanceComparator(typeInfo));

#ifdef DEBUG_DEPENDENCIES
        std::cout << "==BASE TYPE ORDER==" << std::endl;
#endif /* DEBUG_DEPENDENCIES */

        // first level registration - we will create empty elements with correct type info
        for (DataStructures::const_iterator i = found.begin(); i != found.end(); ++i) {
            const std::string& name = i->node->name.symbol.literal;

            const RefractElementFactory& factory = FactoryFromType(typeInfo.ResolveType(i->node));
            auto element = factory.Create(std::string(), eValue);
            element->meta().set("id", from_primitive(name));

            try {
                context.typeRegistry().add(std::move(element));
            } catch (LogicError& e) {
                std::ostringstream out;
                out << name << " is a reserved keyword and cannot be used.";
                throw snowcrash::Error(out.str(), snowcrash::MSONError, i->sourceMap->name.sourceMap);
            }
        }

        for (DataStructures::const_iterator i = found.begin(); i != found.end(); ++i) {

            if (!i->node->name.symbol.literal.empty()) {

                const std::string& name = i->node->name.symbol.literal;
                auto element = MSONToRefract(*i, context);

#ifdef DEBUG_DEPENDENCIES
                TypeQueryVisitor v;
                v.visit(*element);
                std::cout << name << " [" << v.get() << "]" << std::endl;
#endif /* DEBUG_DEPENDENCIES */

                // remove preregistrated element
                context.typeRegistry().remove(name);

                try {
                    context.typeRegistry().add(std::move(element));
                } catch (LogicError& e) {
                    std::ostringstream out;
                    out << name << " is a reserved keyword and cannot be used.";
                    throw snowcrash::Error(out.str(), snowcrash::MSONError, i->sourceMap->name.sourceMap);
                }
            }
        }

#ifdef DEBUG_DEPENDENCIES
        std::cout << "==DEPENDENCIES INFO END==" << std::endl;
#endif /* DEBUG_DEPENDENCIES */
    }
} // ns drafter
