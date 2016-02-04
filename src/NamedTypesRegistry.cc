#include "NamedTypesRegistry.h"

#include <string>
#include <map>
#include <set>

#include "MSON.h"
#include "Blueprint.h"

#include "NodeInfo.h"

#include "refract/Registry.h"
#include <algorithm>

#include "RefractDataStructure.h"

namespace drafter {

    namespace {

        typedef std::vector<NodeInfo<snowcrash::DataStructure> > DataStructures;

        void FindNamedTypes(NodeInfoCollection<snowcrash::Elements>& elements, DataStructures& found)
        {

            for (NodeInfoCollection<snowcrash::Elements>::const_iterator i = elements.begin(); i != elements.end(); ++i) {

                if (i->node->element == snowcrash::Element::DataStructureElement) {
                    found.push_back(MakeNodeInfo(i->node->content.dataStructure, i->sourceMap->content.dataStructure, i->hasSourceMap()));
                }
                else if (!i->node->content.resource.attributes.empty()) {
                    found.push_back(MakeNodeInfo(i->node->content.resource.attributes, i->sourceMap->content.resource.attributes, i->hasSourceMap()));
                }
                else if (i->node->element == snowcrash::Element::CategoryElement) {
                    NodeInfoCollection<snowcrash::Elements> children(MakeNodeInfo(i->node->content.elements(), i->sourceMap->content.elements(), i->hasSourceMap()));
                    FindNamedTypes(children, found);
                }
            }
        }

        struct InheritanceComparator {

            typedef std::map<std::string, std::string> InheritanceMap;
            InheritanceMap childToParent;

            typedef std::set<std::string> Members;
            typedef std::map<std::string, Members> MembersMap;

            MembersMap objectToMembers;

            const std::string& parent(const snowcrash::DataStructure* ds) {
                return ds->typeDefinition.typeSpecification.name.symbol.literal;
            }

            bool hasParent(const snowcrash::DataStructure* ds) {
                return !parent(ds).empty();
            }

            const std::string& name(const snowcrash::DataStructure* ds) {
                return ds->name.symbol.literal;
            }

            const std::string& name(const mson::TypeSpecification& ts) {
                return ts.name.symbol.literal;
            }

            const std::string& name(const mson::ValueDefinition& vd) {
                return name(vd.typeDefinition.typeSpecification);
            }

            const std::string& name(const mson::ValueMember& vm) {
                return name(vm.valueDefinition);
            }

            const std::string& name(const mson::PropertyMember& pm) {
                return name(pm.valueDefinition);
            }

            const std::string& name(const mson::Mixin& mx) {
                return name(mx.typeSpecification);
            }

            Members collectMembers(const mson::Elements& elements) {
                Members members;

                for (mson::Elements::const_iterator it = elements.begin() ; it != elements.end() ; ++it) {

                    std::string member;
                    const mson::TypeSections* ts = NULL;

                    if (!it->content.value.empty() ) {
                        if (!name(it->content.value).empty()) {
                            member = name(it->content.value);
                        }
                        else {
                            ts = &it->content.value.sections;
                        }
                    }
                    else if (!it->content.property.empty()) {
                        if(!name(it->content.property).empty()) {
                            member = name(it->content.property);
                        }
                        else if (!name(it->content.property.name.variable).empty()) {
                            member = name(it->content.property.name.variable);
                        }
                        else {
                            ts = &it->content.property.sections;
                        }
                    }
                    else if (!it->content.mixin.empty()) {
                        if (!name(it->content.mixin).empty()) {
                            member = name(it->content.mixin);
                        }
                    }

                    if (!member.empty()) {
                        members.insert(member);
                    }
                    else if (ts) {
                        Members sub = collectMembers(*ts);
                        members.insert(sub.begin(), sub.end());
                    }
                }

                return members;
            }

            Members collectMembers(const mson::TypeSections& ts) {
                Members members;

                // map direct members
                for (mson::TypeSections::const_iterator its = ts.begin() ; its != ts.end() ; ++its) {
                    Members sub = collectMembers(its->content.elements());
                    members.insert(sub.begin(), sub.end());
                }

                return members;
            }

            Members collectMembers(const snowcrash::DataStructure* ds) {
                return collectMembers(ds->sections);
            }

            InheritanceComparator(const DataStructures& elements) {

                // map inheritance
                for (DataStructures::const_iterator i = elements.begin() ; i != elements.end() ; ++i) {
                    if (hasParent(i->node)) {
                        childToParent[name(i->node)] = parent(i->node);
                    }
                }

                for (DataStructures::const_iterator i = elements.begin() ; i != elements.end() ; ++i) {
                    objectToMembers[name(i->node)] = collectMembers(i->node);
                }
            }

            bool hasMember(const snowcrash::DataStructure* object, const snowcrash::DataStructure* member) {
                MembersMap::const_iterator members = objectToMembers.find(name(object));
                if (members == objectToMembers.end()) {
                    return false;
                }

                return members->second.find(name(member)) != members->second.end();
            }

            bool hasAncestor(const snowcrash::DataStructure* object, const snowcrash::DataStructure* ancestor) {
                std::string s = name(object);
                const std::string& isAncestor = name(ancestor);

                while(!s.empty()) {
                    if (s == isAncestor) {
                        return true;
                    }
                    s = childToParent[s];
                }

                return false;
            }


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
            bool operator()(DataStructures::const_reference first, DataStructures::const_reference second) {

                if (hasAncestor(first.node, second.node) || hasMember(first.node, second.node)) {
                    return false;
                }

                if (hasAncestor(second.node, first.node) || hasMember(second.node, first.node)) {
                    return true;
                }

                return name(first.node) < name(second.node);
            }

        };

    } // ns anonymous

    refract::Registry& GetNamedTypesRegistry()
    {
        static refract::Registry namedTypesRegistry;

        return namedTypesRegistry;
    }

    void RegisterNamedTypes(const NodeInfo<snowcrash::Elements>& elements)
    {
        DataStructures found;
        NodeInfoCollection<snowcrash::Elements> elementCollection(elements);

        FindNamedTypes(elementCollection, found);

        std::sort(found.begin(), found.end(), InheritanceComparator(found));

        for (DataStructures::const_iterator i = found.begin(); i != found.end(); ++i) {

            if (!i->node->name.symbol.literal.empty()) {
                refract::IElement* element = MSONToRefract(*i);
                GetNamedTypesRegistry().add(element);
            }
        }
    }

} // ns drafter
