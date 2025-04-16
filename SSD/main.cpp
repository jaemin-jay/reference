#include "Application.cpp"

void makeCmdList(Application& app) {
	app.addCommand(new ReadCommand("Read", "R", 3));
	app.addCommand(new WriteCommand("Write", "W", 4));
	app.addCommand(new EraseCommand("Erase", "E", 4));
	app.addCommand(new FlushCommand("Flush", "F", 2));
}

int main(int argc, char* argv[]) {
	Application app(new SSD());
	makeCmdList(app);
	app.run(argc, argv);
	return 0;
}
