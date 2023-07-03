using Discord.Net;
using Discord.WebSocket;

public class SlashCommandHandler
{
    public SlashCommandHandler(DiscordSocketClient client)
    {
        client.SlashCommandExecuted += HandleSlashCommandAsync;
    }

    private List<string> SlicedResponse(string Result)
    {
        string[] Lines = Result.Split('\n');

        int Sum = 1900, Pointer = -1;
        List<string> Resultant = new List<string>();
        foreach (string Line in Lines)
        {
            if (Sum + Line.Count() + 1 > 1900) 
            {
                Resultant.Add("");
                Sum = 0;
                Pointer++;
            }
            // Console.WriteLine(Sum);
            Resultant[Pointer] += Line;
            Resultant[Pointer] += '\n';
            Sum += Line.Count() + 1;
        }
        return Resultant;
    }

    public async Task HandleSlashCommandAsync(SocketSlashCommand command)
    {
        string Result = Globals._inputHandler.HandleInput("/" + command.Data.Name);
        Console.WriteLine(Result);
        // Need to slice results
        List<string> SlicedResponses = SlicedResponse(Result);

        for (int i = 0; i < SlicedResponses.Count; i++)
        {
            if (i == 0)
            {
                await command.RespondAsync(SlicedResponses[i]);
            } 
            else 
            {
                await command.Channel.SendMessageAsync(SlicedResponses[i]);
            }
        }
        // await command.Channel.SendMessageAsync("Hi");
        // await command.RespondAsync($"You executed {command.Data.Name} with parameter {command.Data.Options.First().Value}!");
    }
}