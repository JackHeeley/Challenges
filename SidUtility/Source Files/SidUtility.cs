using Pinvoke.Advapi32;
using Pinvoke.Kernel32;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;

namespace Challenge1
{
    /// <summary> static class providing sid string formatters
    /// SidUtilities is a static class presenting sid string formatters required by assignment
    /// encapsulates all sid manipulation stuff and raw byte forms of SID (complexity hiding)
    /// </summary>    
    public class SidUtility
    {
        /// <summary> private helper to convert byte array form of SID to a readable form.
        /// </summary>
        /// <param name="sid">byte[] form of secuity identifier</param>
        /// <returns>string form of security identifier</returns>
        private static string GetSidString(byte[] sid)
        {
            IntPtr ptrSid;
            string sidString;
            if (!Advapi32.ConvertSidToStringSid(sid, out ptrSid))
                throw new System.ComponentModel.Win32Exception();
            try
            {
                sidString = Marshal.PtrToStringAuto(ptrSid);
            }
            finally
            {
                Kernel32.LocalFree(ptrSid);
            }
            return sidString;
        }

        /// <summary>private helper decodes process information to acquire pSID 
        /// </summary>
        /// <param name="processHandle">handle to process</param>
        /// <param name="pSID">pointer to SID</param>
        private static void DumpUserInfo(IntPtr processHandle, out IntPtr pSID)
        {
            UInt32 Access = Advapi32.TOKEN_QUERY;
            IntPtr processToken = IntPtr.Zero;
            pSID = IntPtr.Zero;

            if (!Advapi32.OpenProcessToken(processHandle, Access, out processToken))
                throw new Exception("Can't open process token");

            ProcessTokenToSid(processToken, out pSID);
            Kernel32.CloseHandle(processToken);
        }

        /// <summary>private helper to acquire pSID from Process TOKEN_USER
        /// </summary>
        /// <param name="token">handle of the process token</param>
        /// <param name="pSID">out parameter to receive the pSID</param>
        /// <exception cref="Exception">if shit happens. Probably buffer capacity, access rights</exception>
        private static void ProcessTokenToSid(IntPtr token, out IntPtr pSID)
        {
            Advapi32.TOKEN_USER tokenUser;
            const int bufLength = 256;
            IntPtr tu = Marshal.AllocHGlobal(bufLength);
            pSID = IntPtr.Zero;
            try
            {
                uint cb = bufLength;
                if (!Advapi32.GetTokenInformation(token, Advapi32.TOKEN_INFORMATION_CLASS.TokenUser, tu, cb, out cb))
                    throw new Exception("Can't get token information");

                tokenUser = (Advapi32.TOKEN_USER)Marshal.PtrToStructure(tu, typeof(Advapi32.TOKEN_USER));
                pSID = tokenUser.User.Sid;
            }
            finally
            {
                Marshal.FreeHGlobal(tu);
            }
        }

        /// <summary> private helper to convert the byte array form of a SID to the required formatted string
        /// </summary>
        /// <param name="abySid">byte array form of a SID</param>
        /// <returns>"domain\name : S-xxx..." where domain is the user domain, name is the user name and S-xxx... is the string form of the SID</returns>
        /// <exception cref="Win32Exception">if the platform cannot satisfy the request</exception>
        private static String GetFormattedSid(byte[] abySid)
        {
            StringBuilder name = new StringBuilder();
            uint cchName = (uint)name.Capacity;
            StringBuilder referencedDomainName = new StringBuilder();
            uint cchReferencedDomainName = (uint)referencedDomainName.Capacity;
            Advapi32.SID_NAME_USE sidUse;
            String stringSid;

            try
            {
                if (!Advapi32.LookupAccountSid(null, abySid, name, ref cchName, referencedDomainName, ref cchReferencedDomainName, out sidUse))
                    throw new Win32Exception(Marshal.GetLastWin32Error());
            }
            catch (Win32Exception e)
            {
                if (e.NativeErrorCode != Kernel32.ERROR_INSUFFICIENT_BUFFER)
                    throw e;    // no effort to manage unexpected errors

                name.EnsureCapacity((int)cchName);
                referencedDomainName.EnsureCapacity((int)cchReferencedDomainName);
                if (!Advapi32.LookupAccountSid(null, abySid, name, ref cchName, referencedDomainName, ref cchReferencedDomainName, out sidUse))
                    throw new Win32Exception(Marshal.GetLastWin32Error());
            }

            stringSid = GetSidString(abySid);

            return String.Format(@"{0}\{1} : {2}", referencedDomainName.ToString(), name.ToString(), stringSid);
        }

        /// <summary> GetFormattedSid builds a particular readable form of a SecurityIdentifier
        /// </summary>
        /// <param name="sid">a SecurityIdentifier representing a principal</param>
        /// <returns>"domain\name : S-xxx..." where domain is the user domain, name is the user name and S-xxx... is the string form of the SID</returns>
        /// <exception cref="Win32Exception">if the platform cannot satisfy the request</exception>
        public static String GetFormattedSid(SecurityIdentifier sid)
        {
            byte[] abySid = new byte[sid.BinaryLength];
            sid.GetBinaryForm(abySid, 0);
            return GetFormattedSid(abySid);
        }

        /// <summary> GetFormattedSidByName builds a particular readable form of a SecurityIdentifier
        /// </summary>
        /// <param name="accountName">name of a principal</param>
        /// <returns>"domain\name : S-xxx..." where domain is the user domain, name is the user name and S-xxx... is the string form of the SID</returns>
        public static String GetFormattedSidByName(String accountName)
        {
            byte[] abySid = null;
            uint cbSid = 0;
            StringBuilder referencedDomainName = new StringBuilder();
            uint cchReferencedDomainName = (uint)referencedDomainName.Capacity;
            Advapi32.SID_NAME_USE sidUse;

            try
            {
                if (!Advapi32.LookupAccountName(null, accountName, abySid, ref cbSid, referencedDomainName, ref cchReferencedDomainName, out sidUse))
                    throw new Win32Exception(Marshal.GetLastWin32Error());
            }
            catch (Win32Exception e)
            {
                switch (e.NativeErrorCode)
                {
                    case Kernel32.ERROR_INSUFFICIENT_BUFFER:
                        break;
                    case Kernel32.ERROR_INVALID_FLAGS:
                        break;
                    default:
                        throw e;
                }
            }
            abySid = new byte[cbSid];
            referencedDomainName.EnsureCapacity((int)cchReferencedDomainName);
            if (!Advapi32.LookupAccountName(null, accountName, abySid, ref cbSid, referencedDomainName, ref cchReferencedDomainName, out sidUse))
                throw new Win32Exception(Marshal.GetLastWin32Error());
            return GetFormattedSid(abySid);
        }

        /// <summary>GetFormattedSidByProcess
        /// </summary>
        /// <param name="process">the process to use for identifying the owner sid</param>
        /// <returns>"domain\name : S-xxx..." where domain is the user domain, name is the user name and S-xxx... is the string form of the SID</returns>
        public static string GetFormattedSidByProcess(Process process)
        {
            IntPtr pSID = IntPtr.Zero;
            DumpUserInfo(process.Handle, out pSID);
            return GetFormattedSid(new SecurityIdentifier(pSID));
        }
    }
}
