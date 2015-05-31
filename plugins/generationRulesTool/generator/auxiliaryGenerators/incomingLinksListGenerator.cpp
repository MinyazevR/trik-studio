#include "incomingLinksListGenerator.h"
#include "linksListGenerator.h"

using namespace generationRules::generator;
using namespace simpleParser::ast;

qReal::IdList IncomingLinksListGenerator::generatedList(
		QSharedPointer<Node> linksIdentifierNode
		, QSharedPointer<Identifier> linksTypeNode
		, qReal::LogicalModelAssistInterface *logicalModelInterface
		, VariablesTable variablesTable
		, CurrentScope currentScope)
{
	const auto neededElementId = LinksListGenerator::elementId(linksIdentifierNode, variablesTable, currentScope);
	const auto allIncomingLinks = logicalModelInterface->logicalRepoApi().incomingLinks(neededElementId);
	return LinksListGenerator::linksOfNeededType(linksTypeNode, allIncomingLinks);
}
