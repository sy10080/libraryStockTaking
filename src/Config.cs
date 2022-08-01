public class Config
{
    public string DefaultBooklistLocation;
    public string DefaultSaveLocation;
    public string DefaultProgramFilesLocation;
    public string LoggingLevel;
    public bool ShowAllBooksPosition;
    public bool Autocheck;
    public bool BlockInvalidInputs;
    public bool AutoCapitalize;
    public string[] configs = new string[8] {
        "DefaultBooklistLocation",
        "DefaultSaveLocation",
        "DefaultProgramFilesLocation",
        "LoggingLevel",
        "ShowAllBooksPosition",
        "Autocheck",
        "BlockInvalidInputs",
        "AutoCapitalize"
    };
    // public Type[] configTypes = new Type[5] {typeof(string), typeof(string), typeof(bool), typeof(bool), typeof(bool)};

    public Config()
    {
        DefaultBooklistLocation = "files\\booklist";
        DefaultSaveLocation = "foo";
        DefaultProgramFilesLocation = "files\\";
        LoggingLevel = "Information";
        ShowAllBooksPosition = false;
        Autocheck = false;
        BlockInvalidInputs = false;
        AutoCapitalize = false;
    }
}