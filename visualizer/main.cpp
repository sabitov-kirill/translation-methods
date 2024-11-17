#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include <iostream>
#include <string>
#include <unordered_map>

#include <SyntaxAnalyzer.h>

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
    _nodeIds[newNode] = std::move(uniqueId);

    return newNode;
  }

  Agedge_t *addEdge(Agnode_t *a, Agnode_t *b) {
    return agedge(_g, a, b, 0, 1);
  }

  void render(const std::string &fileName, const std::string &engine = "dot",
              const std::string &format = "png") {
    gvLayout(_gvc, _g, engine.c_str());
    gvRenderFilename(_gvc, _g, format.c_str(), fileName.c_str());
  }

private:
  GVC_t *_gvc;
  Agraph_t *_g;
  std::size_t _nodeIndex{};
  std::unordered_map<Agnode_t *, std::string> _nodeIds;
};

void astToAg(GraphManager &g, NameASTNode &parent, Agnode_t *parentNode) {
  for (auto &&child : parent.children) {
    Agnode_t *childNode = g.addNode(child.data);
    g.addEdge(parentNode, childNode);
    astToAg(g, child, childNode);
  }
}

void astToAg(GraphManager &g, NameASTNode &root) {
  Agnode_t *rootNode = g.addNode(root.data);
  astToAg(g, root, rootNode);
}

} // namespace

int main(int argc, char *argv[]) {
  if (argc != 3)  {
    std::cerr << "Wrong usage. Use: " << argv[0] << " [expression] [output file]\n";
    return 1;
  }

  StringSource ss(argv[1]);
  LexicalAnalyzer<StringSource> la(std::move(ss));
  StringSyntaxAnalyzer sa(la);
  auto astRoot = sa.parse();

  GraphManager g;
  astToAg(g, astRoot);
  g.render(argv[2]);
}
