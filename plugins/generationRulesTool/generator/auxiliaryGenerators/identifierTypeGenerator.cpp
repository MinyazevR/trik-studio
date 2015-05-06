#include "identifierTypeGenerator.h"

#include "ast/identifier.h"
#include "ast/incomingLinks.h"
#include "ast/outcomingLinks.h"
#include "ast/elementIdentifier.h"

using namespace generationRules::generator;
using namespace simpleParser::ast;

QString IdentifierTypeGenerator::variableType(QSharedPointer<List> listNode)
{
	auto mainPartOfListIdentifier = qrtext::as<ElementIdentifier>(listNode->identifier());
	auto identifierPart = qrtext::as<Identifier>(mainPartOfListIdentifier->identifierPart());
	auto optionalLinkPart = listNode->optionalLinkPart();

	// TODO: rewrite this code.
	if (!optionalLinkPart) {
		return identifierPart->name();
	} else {
		if (optionalLinkPart->is<IncomingLinks>()) {
			auto linkTypeNode = qrtext::as<IncomingLinks>(optionalLinkPart)->linkType();
			auto linkTypeIdentifier = qrtext::as<Identifier>(linkTypeNode);

			return linkTypeIdentifier->name();
		} else {
			if (optionalLinkPart->is<OutcomingLinks>()) {
				auto linkTypeNode = qrtext::as<OutcomingLinks>(optionalLinkPart)->linkType();
				auto linkTypeIdentifier = qrtext::as<Identifier>(linkTypeNode);

				return linkTypeIdentifier->name();
			}
		}
	}
}
