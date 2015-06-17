//
//  refract/ExpandVisitor.cc
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "Element.h"
#include "Visitors.h"
#include "Registry.h"

namespace refract
{
    IElement* ExpandVisitor::expandOrClone(const IElement* e) {
        IElement* result = NULL;
        if(!e) {
            return result;
        }

        ExpandVisitor expander(registry);
        e->content(expander);
        result = expander.get();

        if(!result) {
            result = e->clone();
        }

        return result;
    }

    ExpandVisitor::ExpandVisitor(const Registry& registry) : result(NULL), registry(registry) {};

    void ExpandVisitor::visit(const IElement& e) {

        IsExpandableVisitor isExpandable;
        e.content(isExpandable);
        if (!isExpandable.get()) {
            return;
        }

        e.content(*this);
    }

    void ExpandVisitor::visit(const MemberElement& e) {
        //std::cout << __PRETTY_FUNCTION__ <<  std::endl;

        IsExpandableVisitor isExpandable;
        e.content(isExpandable);
        if (!isExpandable.get()) {
            return;
        }

        MemberElement* expanded = static_cast<MemberElement*>(e.clone(IElement::cAll ^ IElement::cValue));

        expanded->set(expandOrClone(e.value.first), expandOrClone(e.value.second));

        result = expanded;
    }

    void ExpandVisitor::visit(const ObjectElement& e) {
        //std::cout << __PRETTY_FUNCTION__ <<  std::endl;

        std::string en = e.element();

        // FIXME: refactoring - split into method
        if(!isReserved(en)) { // handle direct inheritance
            refract::ObjectElement* o = new refract::ObjectElement;
            o->element("extend");

            IElement::MemberElementCollection::const_iterator name = e.meta.find("id");
            if (name != e.meta.end() && (*name)->value.second) {
                o->meta["id"] = (*name)->value.second->clone();
            }

            // go as deep as possible in inheritance tree
            for (const IElement* parent = registry.find(en)
                ; parent && !isReserved(en)
                ; en = parent->element(), parent = registry.find(en) ) {

                // FIXME: while clone original element w/o meta - we lose `description`
                // must be fixed in spec
                IElement* clone = parent->clone(IElement::cAll ^ IElement::cMeta);
                clone->meta["ref"] = IElement::Create(en);
                o->push_back(clone);
            }

            ObjectElement* origin = new ObjectElement; // warapper for original object
            for (ObjectElement::ValueType::const_iterator it = e.value.begin() ; it != e.value.end() ; ++it) {
                origin->push_back(expandOrClone(*it));
            }
            o->push_back(origin);

            result = o;
            return;
        }
        else if (en == "ref") {
            //refract::ObjectElement* o = new refract::ObjectElement;
            refract::ObjectElement* o = new refract::ObjectElement;

            for (ObjectElement::ValueType::const_iterator it = e.value.begin()
                ; it != e.value.end()
                ; ++it ) {

                ComparableVisitor href("href", ComparableVisitor::key);
                (*it)->content(href);

                if(href.get()) { // key was recognized - it is save to cast to MemberElement
                    MemberElement* m = static_cast<MemberElement*>(*it);
                    if(m->value.second) {
                        TypeQueryVisitor tq;
                        if(StringElement* value = tq.as<StringElement>(m->value.second)) {
                            en = value->value;

                            // go as deep as possible in inheritance tree
                            for (const IElement* parent = registry.find(en)
                               ; parent && !isReserved(en)
                               ; en = parent->element(), parent = registry.find(en) ) {

                               // FIXME: while clone original element w/o meta - we lose `description`
                               // must be fixed in spec
                               IElement* clone = parent->clone(IElement::cAll ^ IElement::cMeta);
                               clone->meta["ref"] = IElement::Create(en);
                               o->push_back(clone);
                            }
                        }
                    }
                }
            }

            result = o;
            return;
        }

        // handle expandable members
        IsExpandableVisitor isExpandable;
        e.content(isExpandable);
        if (!isExpandable.get()) {
           //std::cout << "not expandable - leave" << std::endl;
            return;
        }

        // do not clone value, we have to do it one by one because some of them must be expanded
        refract::ObjectElement* expanded = static_cast<ObjectElement*>(e.clone(IElement::cAll ^ IElement::cValue));

        std::vector<IElement*> members;
        bool hasRef = false;

        for (ObjectElement::ValueType::const_iterator it = e.value.begin() ; it != e.value.end() ; ++it) {
            if((*it)->element() == "ref") {
               hasRef = true; 
            }

            members.push_back(expandOrClone(*it));
        }

        expanded->set(members);

        if (hasRef) {
            ObjectElement* extend = new ObjectElement;
            extend->element("extend");
            extend->push_back(expanded);
            expanded = extend;
        }

        result = expanded;
    }

    IElement* ExpandVisitor::get() const {
        return result;
    }

}; // namespace refract
