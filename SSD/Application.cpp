#include "SSD.cpp"
#include "Command.cpp"
#include <vector>

class Application {
private:
	ISSD* ssd;
	vector<Command*> cmdList;

	Command *findCommand(string commandName) {
		Command* ret = nullptr;
		for (int i = 0; i < cmdList.size(); ++i) {
			if (cmdList[i]->getCommandName() != commandName)
				continue;
			ret = cmdList[i];
			break;
		}
		return ret;
	}

public:
	Application(ISSD* ssd) : ssd(ssd) {
		cmdList.clear();
	}

	void addCommand(Command* cmd) {
		cmdList.push_back(cmd);
	}

	void run(int argc, char* argv[]) {
		if (argc < 2) {
			cout << "[Error] Need more parameter!" << endl;
			return;
		}

		Command* cmd = findCommand(argv[1]);
		if (cmd == nullptr) {
			cout << "[Error] Can't find command! " << argv[1] << endl;
			return;
		}

		if (!cmd->isCorrectArgCount(argc)) {
			cout << "[Error] Invalid parameters!" << endl;
			return;
		}

		try {
			cmd->execute(ssd, argv);
		}
		catch (exception& e) {
			cout << e.what() << endl;
			cout << "[Error] Fail " << cmd->getTitle() << "!" << endl;
		}
	}
};
