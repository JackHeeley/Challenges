using System;
using System.ComponentModel;
using System.Security.Principal;

namespace Challenge1
{
    class CMainProgram
    {
 
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                Console.WriteLine(SidUtility.GetFormattedSid(WindowsIdentity.GetCurrent().User));
            }
            catch (Win32Exception w)
            {
                Console.WriteLine(@"Win32Exception ErrorCode: {0} Message: {1}", w.NativeErrorCode, w.Message);
            }
            catch(Exception e)
            {
                Console.WriteLine(@"Last ditch exception: {0}", e.Message);
            }
        }
    }
}
