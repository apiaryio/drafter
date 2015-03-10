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

const std::string SerializeKey::ASTVersion = "_version";
const std::string SerializeKey::Metadata = "metadata";
const std::string SerializeKey::Reference = "reference";
const std::string SerializeKey::Id = "id";
const std::string SerializeKey::Name = "name";
const std::string SerializeKey::Description = "description";
const std::string SerializeKey::ResourceGroups = "resourceGroups";
const std::string SerializeKey::Resources = "resources";
const std::string SerializeKey::URI = "uri";
const std::string SerializeKey::URITemplate = "uriTemplate";
const std::string SerializeKey::Assets = "assets";
const std::string SerializeKey::Actions = "actions";
const std::string SerializeKey::Action = "action";
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

const std::string SerializeKey::AnnotationsVersion = "_version";
const std::string SerializeKey::Ast = "ast";
const std::string SerializeKey::SourceMap= "sourcemap";
const std::string SerializeKey::Error = "error";
const std::string SerializeKey::Warnings = "warnings";
const std::string SerializeKey::AnnotationCode = "code";
const std::string SerializeKey::AnnotationMessage = "message";
const std::string SerializeKey::AnnotationLocation = "location";
const std::string SerializeKey::AnnotationLocationIndex = "index";
const std::string SerializeKey::AnnotationLocationLength = "length";
