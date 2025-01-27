#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.h"
#include "parser.h"

namespace {

char GM_GRAP_NAME[] = "g";
char GM_LABEL[] = "label";

class GraphManager {
public:
  GraphManager() : _gvc(gvContext()), _g(agopen(GM_GRAP_NAME, Agdirected, 0)) {}
  ~GraphManager() {
    gvFreeLayout(_gvc, _g);
    gvFreeContext(_gvc);
    agclose(_g);
  }
  Agnode_t *addNode(const std::string &name) {
    std::string uniqueId = std::to_string(_nodeIndex++);
    Agnode_t *newNode = agnode(_g, const_cast<char *>(uniqueId.c_str()), 1);
    agset(newNode, GM_LABEL, const_cast<char *>(name.c_str()));
    return newNode;
  }
  Agedge_t *addEdge(Agnode_t *a, Agnode_t *b) { return agedge(_g, a, b, 0, 1); }
  void render(const std::string &fileName, const std::string &engine = "dot",
              const std::string &format = "png") {
    gvLayout(_gvc, _g, engine.c_str());
    gvRenderFilename(_gvc, _g, format.c_str(), fileName.c_str());
  }

private:
  GVC_t *_gvc;
  Agraph_t *_g;
  std::size_t _nodeIndex{};
};

void astToAg(GraphManager &mgr, NameASTNode &node, Agnode_t *parentNode) {
  for (auto &child : node.children) {
    Agnode_t *childNode = mgr.addNode(child.data);
    mgr.addEdge(parentNode, childNode);
    astToAg(mgr, child, childNode);
  }
}

void astToAg(GraphManager &mgr, NameASTNode &root) {
  Agnode_t *rootNode = mgr.addNode(root.data);
  astToAg(mgr, root, rootNode);
}

} // namespace

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " [expression] [output file]\n";
    return 1;
  }

  std::istringstream input(argv[1]);
  Lexer lexer(&input);
  Parser parser(lexer);
  NameASTNode astRoot = parser.parse();

  GraphManager gm;
  astToAg(gm, astRoot);
  gm.render(argv[2]);

  return 0;
}