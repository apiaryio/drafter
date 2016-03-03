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

#define DEBUG_DEPENDENCIES

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

            const std::string& name(const mson::TypeName& tn) {
                return tn.symbol.literal;
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
                    const mson::TypeNames* tn = NULL;

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
                        else if (!it->content.property.valueDefinition.typeDefinition.typeSpecification.nestedTypes.empty()) {
                            // - name (array|enum[<nested>, <type>])
                            tn = &it->content.property.valueDefinition.typeDefinition.typeSpecification.nestedTypes;
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
                    else if (tn) {
                        Members sub = collectMembers(*tn);
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

            Members collectMembers(const mson::TypeNames& tn) {
                Members members;

                for (mson::TypeNames::const_iterator itn = tn.begin() ; itn != tn.end() ; ++itn) {
                    members.insert(name(*itn));
                }

                return members;
            }

            Members collectMembers(const snowcrash::DataStructure* ds) {
                return collectMembers(ds->sections);
            }

            Members collectMembers(const std::string& id, const MembersMap& members, Members& resolved) {
                Members collected;

                if (resolved.find(id) != resolved.end()) {
                    return collected;
                }

                resolved.insert(id);

                MembersMap::const_iterator member = members.find(id);
                if (member == members.end()) {
                    return collected;
                }

                for (Members::const_iterator i = member->second.begin() ; i != member->second.end() ; ++i) {
                    Members sub = collectMembers(*i, members, resolved);
                    collected.insert(*i);
                    collected.insert(sub.begin(), sub.end());
                }

                return collected;
            }

            InheritanceComparator(const DataStructures& elements) {

                // map inheritance
                for (DataStructures::const_iterator i = elements.begin() ; i != elements.end() ; ++i) {
                    if (hasParent(i->node)) {
                        childToParent[name(i->node)] = parent(i->node);
#ifdef DEBUG_DEPENDENCIES
                        std::cout << "Parent: " << name(i->node) << "=>" << parent(i->node) << std::endl;
#endif
                    }
                }

                // Map members
                MembersMap members;
                for (DataStructures::const_iterator i = elements.begin() ; i != elements.end() ; ++i) {
                    members[name(i->node)] = collectMembers(i->node);
                }

                // Map members into deep
                for (MembersMap::const_iterator i = members.begin() ; i != members.end() ; ++i) {
                    Members resolved;
                    objectToMembers[i->first] = collectMembers(i->first, members, resolved);
                }

#ifdef DEBUG_DEPENDENCIES
                // debug out members
                for (MembersMap::const_iterator i = objectToMembers.begin() ; i != objectToMembers.end() ; ++i) {
                    std::cout << "Members: " << i->first << std::endl;
                    for (Members::const_iterator it = objectToMembers[i->first].begin() ; it != objectToMembers[i->first].end() ; ++it) {
                        std::cout << " - " << *it << std::endl;
                    }
                }
#endif /* DEBUG_DEPENDENCIES */

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

#ifdef DEBUG_DEPENDENCIES
        std::cout << "==DEPENDENCIES INFO BEGIN==" << std::endl;
#endif /* DEBUG_DEPENDENCIES */

        std::sort(found.begin(), found.end(), InheritanceComparator(found));

#ifdef DEBUG_DEPENDENCIES
                std::cout << "==BASE TYPE ORDER==" << std::endl;
#endif /* DEBUG_DEPENDENCIES */

        for (DataStructures::const_iterator i = found.begin(); i != found.end(); ++i) {

            if (!i->node->name.symbol.literal.empty()) {

                refract::IElement* element = MSONToRefract(*i);
#ifdef DEBUG_DEPENDENCIES
                refract::TypeQueryVisitor v;
                v.visit(*element);
                std::cout 
                    << i->node->name.symbol.literal 
                    << " ["
                    << v.get()
                    << "]"
                    << std::endl;
#endif /* DEBUG_DEPENDENCIES */

                GetNamedTypesRegistry().add(element);
            }
        }

#ifdef DEBUG_DEPENDENCIES
        std::cout << "==DEPENDENCIES INFO END==" << std::endl;
#endif /* DEBUG_DEPENDENCIES */

    }

} // ns drafter
