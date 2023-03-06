#pragma once

class MessageHandler
{
public:
	MessageHandler() = default;
	~MessageHandler() = default;

	static struct DebugMessages 
	{
		std::unordered_map<string, unsigned int> myMessagesAmmount;
		std::unordered_map<string, unsigned int> myErrosAmmount;
		std::vector<string> messages;
		std::vector<string> errors;
		bool myCollapseToggle = true;

		void AddMessage(string msg) 
		{
			if (myCollapseToggle) 
			{
				if (myMessagesAmmount.find(msg) == myMessagesAmmount.end())
				{
					myMessagesAmmount.insert({ msg, 1 });
					messages.push_back(msg);
				}
				else
				{
					myMessagesAmmount[msg]++;
				}
			}
			else 
			{
				messages.push_back(msg);
			}
		}
	} debugMessages;


private:
};
//
//static void print(const char* aMessage)
//{
//	MessageHandler::debugMessages.messages.push_back(aMessage);
//}
//
//static void printerror(const char* aMessage)
//{
//	MessageHandler::debugMessages.errors.push_back(aMessage);
//}