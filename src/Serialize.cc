//
//  Serialize.cc
//  drafter
//
//  Created by Zdenek Nemec on 5/3/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#include "Serialize.h"
#include "StringUtility.h"

using namespace drafter;

#ifdef _WITH_REFRACT_
namespace drafter {

    /**
     * Proxy function to retrieve the NamedTypesRegistry
     * NOTE: DO NOT USE THIS
     */
    refract::Registry& GetNamedTypesRegistry()
    {
        static refract::Registry namedTypesRegistry;

        return namedTypesRegistry;
    }
}
#endif

const std::string SerializeKey::Metadata = "metadata";
const std::string SerializeKey::Reference = "reference";
const std::string SerializeKey::Id = "id";
const std::string SerializeKey::Name = "name";
const std::string SerializeKey::Description = "description";
const std::string SerializeKey::DataStructure = "dataStructure";
const std::string SerializeKey::DataStructures = "dataStructures";
const std::string SerializeKey::ResourceGroup = "resourceGroup";
const std::string SerializeKey::ResourceGroups = "resourceGroups";
const std::string SerializeKey::Resource = "resource";
const std::string SerializeKey::Resources = "resources";
const std::string SerializeKey::URI = "uri";
const std::string SerializeKey::URITemplate = "uriTemplate";
const std::string SerializeKey::Assets = "assets";
const std::string SerializeKey::Actions = "actions";
const std::string SerializeKey::Action = "action";
const std::string SerializeKey::Relation = "relation";
const std::string SerializeKey::Attributes = "attributes";
const std::string SerializeKey::Method = "method";
const std::string SerializeKey::Examples = "examples";
const std::string SerializeKey::Requests = "requests";
const std::string SerializeKey::Responses = "responses";
const std::string SerializeKey::Body = "body";
const std::string SerializeKey::Schema = "schema";
const std::string SerializeKey::Headers = "headers";
const std::string SerializeKey::Model = "model";
const std::string SerializeKey::Value = "value";
const std::string SerializeKey::Parameters = "parameters";
const std::string SerializeKey::Type = "type";
const std::string SerializeKey::Required = "required";
const std::string SerializeKey::Default = "default";
const std::string SerializeKey::Nullable = "nullable";
const std::string SerializeKey::Example = "example";
const std::string SerializeKey::Values = "values";

const std::string SerializeKey::Source = "source";
const std::string SerializeKey::Resolved = "resolved";

const std::string SerializeKey::Literal = "literal";
const std::string SerializeKey::Variable = "variable";
const std::string SerializeKey::TypeDefinition = "typeDefinition";
const std::string SerializeKey::TypeSpecification = "typeSpecification";
const std::string SerializeKey::NestedTypes = "nestedTypes";
const std::string SerializeKey::Sections = "sections";
const std::string SerializeKey::Class = "class";
const std::string SerializeKey::Content = "content";
const std::string SerializeKey::ValueDefinition = "valueDefinition";

const std::string SerializeKey::Element = "element";
const std::string SerializeKey::Role = "role";

const std::string SerializeKey::Version = "_version";
const std::string SerializeKey::Ast = "ast";
const std::string SerializeKey::SourceMap= "sourcemap";
const std::string SerializeKey::Error = "error";
const std::string SerializeKey::Warnings = "warnings";
const std::string SerializeKey::AnnotationCode = "code";
const std::string SerializeKey::AnnotationMessage = "message";
const std::string SerializeKey::AnnotationLocation = "location";
const std::string SerializeKey::AnnotationLocationIndex = "index";
const std::string SerializeKey::AnnotationLocationLength = "length";

const std::string SerializeKey::Meta = "meta";
const std::string SerializeKey::Title = "title";
const std::string SerializeKey::Classes = "classes";
const std::string SerializeKey::Samples = "samples";
const std::string SerializeKey::TypeAttributes = "typeAttributes";
const std::string SerializeKey::Optional = "optional";
const std::string SerializeKey::Fixed = "fixed";
const std::string SerializeKey::True = "true";
const std::string SerializeKey::Generic = "generic";
const std::string SerializeKey::Enum = "enum";
const std::string SerializeKey::Select = "select";
const std::string SerializeKey::Option = "option";
const std::string SerializeKey::Ref = "ref";
const std::string SerializeKey::Href = "href";
const std::string SerializeKey::Path = "path";

const std::string SerializeKey::Category = "category";
const std::string SerializeKey::Copy = "copy";
const std::string SerializeKey::API = "api";
const std::string SerializeKey::User = "user";
const std::string SerializeKey::Transition = "transition";
const std::string SerializeKey::HrefVariables = "hrefVariables";
const std::string SerializeKey::HTTPHeaders = "httpHeaders";
const std::string SerializeKey::HTTPTransaction = "httpTransaction";
const std::string SerializeKey::ContentType = "contentType";
const std::string SerializeKey::HTTPResponse = "httpResponse";
const std::string SerializeKey::HTTPRequest = "httpRequest";
const std::string SerializeKey::StatusCode = "statusCode";
const std::string SerializeKey::Asset = "asset";
const std::string SerializeKey::MessageBody = "messageBody";
const std::string SerializeKey::MessageSchema = "messageSchema";
const std::string SerializeKey::Data = "data";
