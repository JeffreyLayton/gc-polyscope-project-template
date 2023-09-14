#include "geometrycentral/surface/manifold_surface_mesh.h"
#include "geometrycentral/surface/meshio.h"
#include "geometrycentral/surface/vertex_position_geometry.h"

#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"

#include "args/args.hxx"
#include "imgui.h"

using namespace geometrycentral;
using namespace geometrycentral::surface;

// == Geometry-central data
std::unique_ptr<ManifoldSurfaceMesh> mesh;
std::unique_ptr<VertexPositionGeometry> geometry;

// Polyscope visualization handle, to quickly add data to the surface
polyscope::SurfaceMesh* psMesh;
polyscope::SurfaceVertexScalarQuantity* psVertexMinimumPrincipleCurvature;
polyscope::SurfaceVertexScalarQuantity* psVertexMaximumPrincipleCurvature;
polyscope::SurfaceVertexScalarQuantity* psVertexMeanCurvature;
polyscope::SurfaceVertexScalarQuantity* psVertexGaussianCurvature;
polyscope::SurfaceFaceScalarQuantity* psFaceGaussianCurvature;

void myCallback() {
	//if (ImGui::Combo("Curvature options", &item_current, items, IM_ARRAYSIZE(items)) {
	//}
}

int main(int argc, char** argv) {
	// Configure the argument parser
	args::ArgumentParser parser("geometry-central & Polyscope example project");
	args::Positional<std::string> inputFilename(parser, "mesh", "A mesh file.");

	// Parse args
	try {
		parser.ParseCLI(argc, argv);
	}
	catch (args::Help& h) {
		std::cout << parser;
		return 0;
	}
	catch (args::ParseError& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	// Make sure a mesh name was given
	if (!inputFilename) {
		std::cerr << "Please specify a mesh file as argument" << std::endl;
		return EXIT_FAILURE;
	}

	// Initialize polyscope
	polyscope::init();

	// Set the callback function
	polyscope::state::userCallback = myCallback;

	// Load mesh
	std::tie(mesh, geometry) = readManifoldSurfaceMesh(args::get(inputFilename));

	// Register the mesh with polyscope
	psMesh = polyscope::registerSurfaceMesh(
		polyscope::guessNiceNameFromPath(args::get(inputFilename)),
		geometry->inputVertexPositions, mesh->getFaceVertexList(),
		polyscopePermutations(*mesh));

	// Set vertex tangent spaces
	geometry->requireVertexGaussianCurvatures();
	geometry->requireVertexMeanCurvatures();
	geometry->requireVertexMinPrincipalCurvatures();
	geometry->requireVertexMaxPrincipalCurvatures();
	geometry->requireFaceGaussianCurvatures();

	psVertexMinimumPrincipleCurvature = psMesh->addVertexScalarQuantity("Minimum Principle Vertex Curvature", geometry->vertexMinPrincipalCurvatures);
	psVertexMaximumPrincipleCurvature = psMesh->addVertexScalarQuantity("Maximum Principle Vertex Curvature", geometry->vertexMaxPrincipalCurvatures);
	psVertexMeanCurvature = psMesh->addVertexScalarQuantity("Mean Vertex Curvature", geometry->vertexMeanCurvatures);
	psVertexGaussianCurvature = psMesh->addVertexScalarQuantity("Gaussian Vertex Curvature", geometry->vertexGaussianCurvatures);
	psFaceGaussianCurvature = psMesh->addFaceScalarQuantity("Gaussian Face Curvature", geometry->faceGaussianCurvatures);

	// Give control to the polyscope gui
	polyscope::show();



	return EXIT_SUCCESS;
}