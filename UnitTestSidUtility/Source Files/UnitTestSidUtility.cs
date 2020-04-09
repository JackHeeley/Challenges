using Challenge1;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Pinvoke.Kernel32;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Management;
using System.Security.Principal;
using System.ServiceProcess;
using System.Threading;

namespace UnitTestChallenges
{
    [TestClass]
    public class UnitTestChallenge1
    {
        [TestMethod]
        public void TestErrorFlow()
        {
            try
            {
                System.Security.Principal.SecurityIdentifier sid = null;
                SidUtility.GetFormattedSid(sid);
                Assert.Fail("GetFormattedSid didn't throw expected exception");
            }
            catch (Exception e)
            { 
                Console.WriteLine(String.Format("Expected exception : {0}", e.Message));
            }
        }

        [TestMethod]
        public void TestHappyFlow()
        {
            try
            {
                SecurityIdentifier sid = WindowsIdentity.GetCurrent().User;
                String s = SidUtility.GetFormattedSid(sid);
                Console.WriteLine(String.Format("Current User SID: {0}", s));
            }
            catch (Exception e)
            {
                Assert.Fail(String.Format("GetFormattedSid threw an unexpected exception: {0}", e.Message));
            }
        }

        [TestMethod]
        public void TestSystemErrorCodes()
        {
            //just sample to confirm no (obvious) typo's in the enum
            Assert.AreEqual(Kernel32.ERROR_INVALID_HANDLE, 6);
            Assert.AreEqual(Kernel32.ERROR_DUP_NAME, 52);
            Assert.AreEqual(Kernel32.ERROR_NO_PROC_SLOTS, 89);
            Assert.AreEqual(Kernel32.ERROR_BUFFER_OVERFLOW, 111);
            Assert.AreEqual(Kernel32.ERROR_PROCESS_MODE_NOT_BACKGROUND, 403);
        }

        [TestMethod]
        public void TestWithThreadOwnerSid() // Test the function by giving the SID of the current thread as input.
        {
            try
            {
                String sidStr = SidUtility.GetFormattedSidByName(Thread.CurrentPrincipal.Identity.Name);
                Console.WriteLine(String.Format("Current User SID: {0}", sidStr));
            }
            catch (Exception e)
            {
                Assert.Fail(String.Format("GetFormattedSid threw an unexpected exception: {0}", e.Message));
            }
        }

        [TestMethod]
        public void TestWithServiceSid()         // Test with SID of a service that runs under another user.​
        {

            // get list of Windows services
            ServiceController[] services = ServiceController.GetServices();

            try
            {
                foreach (ServiceController scTemp in ServiceController.GetServices())
                {
                    if (scTemp.Status == ServiceControllerStatus.Stopped)
                        continue;    // stopped, so no process ID!

                    ManagementObject service = new ManagementObject(@"Win32_Service.Name='" + scTemp.ServiceName + "'");
                    object o = service.GetPropertyValue("ProcessId");
                    int processId = (int)((UInt32)o);
                    Process process = Process.GetProcessById(processId);

                    String sidStr = SidUtility.GetFormattedSidByProcess(process);
                    Console.WriteLine(String.Format("Current User SID: {0}", sidStr));
                    break; // just do one
                }
            }
            catch (Win32Exception w)
            {
                if (w.NativeErrorCode == Kernel32.ERROR_ACCESS_DENIED)
                {
                    Assert.Inconclusive("This test must run elevated (run VisualStudio as administrator)");
                }
                else
                {
                    Assert.Fail(String.Format("GetFormattedSidFromProcess threw an unexpected exception: {0}", w.Message));
                }
            }
            catch (Exception e)
            {
                Assert.Fail(String.Format("GetFormattedSidFromProcess threw an unexpected exception: {0}", e.Message));
            }
        }
    }
}
