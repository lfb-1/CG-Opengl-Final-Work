std::string GetDirectory (const std::string& path)
{
    size_t found = path.find_last_of("/\\");
    return(path.substr(0, found));
}
