#include "RefractElementFactory.h"
#include "refract/Element.h"
#include "ElementData.h"

#include "Serialize.h"        // LiteralTo<>
#include "SourceAnnotation.h" // mson::Error

namespace drafter
{
    using namespace refract;

    template <typename E, bool IsPrimitive = is_primitive<E>()>
    struct RefractElementFactoryImpl;

    template <typename E>
    struct RefractElementFactoryImpl<E, true> : RefractElementFactory {

        typedef typename E::ValueType ValueType;

        RefractElementFactoryImpl() {}

        virtual std::unique_ptr<IElement> Create(const std::string& literal, FactoryCreateMethod method) const
        {
            using namespace refract;

            if (literal.empty()) {
                return make_empty<E>();
            }

            switch (method) {
                case eSample: {
                    std::pair<bool, ValueType> value = LiteralTo<ValueType>(literal);

                    auto element = make_empty<E>();

                    element->attributes().set(SerializeKey::Samples,
                        value.first ? make_element<ArrayElement>(make_element<E>(value.second)) :
                                      make_empty<ArrayElement>());

                    return std::move(element);
                }

                case eValue: {
                    std::pair<bool, ValueType> value = LiteralTo<ValueType>(literal);
                    return value.first ? //
                        make_element<E>(value.second) :
                        make_empty<E>();
                }

                case eElement: {
                    auto element = make_empty<E>();
                    element->element(literal);
                    return std::move(element);
                }
                default:
                    assert(false);
                    return nullptr;
            }
        }
    };

    template <typename E>
    struct RefractElementFactoryImpl<E, false> : RefractElementFactory {

        RefractElementFactoryImpl() {}

        virtual std::unique_ptr<IElement> Create(const std::string& literal, FactoryCreateMethod method) const
        {
            if (method == eSample) {
                auto element = make_element<StringElement>(literal);
                element->element(SerializeKey::Generic);
                return std::move(element);
            }

            auto element = make_empty<E>();

            if (!literal.empty()) {
                element->element(literal);
            }

            return std::move(element);
        }
    };

    const RefractElementFactory& FactoryFromType(const mson::BaseTypeName typeName)
    {

        static const RefractElementFactoryImpl<BooleanElement> boolFactory;
        static const RefractElementFactoryImpl<NumberElement> numberFactory;
        static const RefractElementFactoryImpl<StringElement> stringFactory;
        static const RefractElementFactoryImpl<EnumElement> enumFactory;
        static const RefractElementFactoryImpl<ArrayElement> arrayFactory;
        static const RefractElementFactoryImpl<ObjectElement> objectFactory;

        switch (typeName) {
            case mson::BooleanTypeName:
                return boolFactory;
            case mson::NumberTypeName:
                return numberFactory;
            case mson::StringTypeName:
                return stringFactory;
            case mson::ArrayTypeName:
                return arrayFactory;
            case mson::EnumTypeName:
                return enumFactory;
            case mson::ObjectTypeName:
            case mson::UndefinedTypeName:
                return objectFactory;
            default:; // do nothing
        }

        throw snowcrash::Error("unknown mson type", snowcrash::ApplicationError);
    }
}
